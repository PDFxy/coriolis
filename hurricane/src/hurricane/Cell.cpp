// ****************************************************************************************************
// File: Cell.cpp
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#include "hurricane/Cell.h"
#include "hurricane/DataBase.h"
#include "hurricane/Library.h"
#include "hurricane/Instance.h"
#include "hurricane/Net.h"
#include "hurricane/Pin.h"
#include "hurricane/RoutingPad.h"
#include "hurricane/Layer.h"
#include "hurricane/Slice.h"
#include "hurricane/Rubber.h"
#include "hurricane/Marker.h"
#include "hurricane/Component.h"
#include "hurricane/UpdateSession.h"
#include "hurricane/Error.h"

namespace Hurricane {

// ****************************************************************************************************
// Cell implementation
// ****************************************************************************************************

Cell::Cell(Library* library, const Name& name)
// *******************************************
:    Inherit(),
    _library(library),
    _name(name),
    _instanceMap(),
    _quadTree(),
    _slaveInstanceSet(),
    _netMap(),
    _sliceMap(),
    _markerSet(),
    //_viewSet(),
    _abutmentBox(),
    _boundingBox(),
    _isTerminal(true),
    _isPad(false),
    _nextOfLibraryCellMap(NULL),
    _nextOfSymbolCellSet(NULL),
    _slaveEntityMap()
{
    if (!_library)
        throw Error("Can't create " + _TName("Cell") + " : null library");

    if (name.isEmpty())
        throw Error("Can't create " + _TName("Cell") + " : empty name");

    if (_library->getCell(_name))
        throw Error("Can't create " + _TName("Cell") + " " + getString(_name) + " : already exists");
}

Cell* Cell::create(Library* library, const Name& name)
// ***************************************************
{
    Cell* cell = new Cell(library, name);

    cell->_postCreate();

    return cell;
}

Box Cell::getBoundingBox() const
// *****************************
{
    if (_boundingBox.isEmpty()) {
        Box& boundingBox = (Box&)_boundingBox;
        boundingBox = _abutmentBox;
        boundingBox.merge(_quadTree.getBoundingBox());
        for_each_slice(slice, getSlices()) {
            boundingBox.merge(slice->getBoundingBox());
            end_for;
        }
    }
    
    return _boundingBox;
}

bool Cell::isLeaf() const
// **********************
{
    return _instanceMap.IsEmpty();
}

bool Cell::isCalledBy(Cell* cell) const
// ************************************
{
    for_each_instance(instance, cell->getInstances()) {
        Cell* masterCell = instance->getMasterCell();
        if (masterCell == this) return true;
        if (isCalledBy(masterCell)) return true;
        end_for;
    }
    return false;
}

void Cell::setName(const Name& name)
// *********************************
{
    if (name != _name) {
        if (name.isEmpty())
            throw Error("Can't change " + _TName("Cell") + " name : empty name");

        if (_library->getCell(name))
            throw Error("Can't change " + _TName("Cell") + " name : already exists");

        _library->_getCellMap()._remove(this);
        _name = name;
        _library->_getCellMap()._insert(this);
    }
}

void Cell::setAbutmentBox(const Box& abutmentBox)
// **********************************************
{
    if (abutmentBox != _abutmentBox) {
        if (!_abutmentBox.isEmpty() &&
             (abutmentBox.isEmpty() || !abutmentBox.contains(_abutmentBox)))
            _unfit(_abutmentBox);
        _abutmentBox = abutmentBox;
        _fit(_abutmentBox);
    }
}

void Cell::flattenNets(bool buildRings)
// ************************************
{
    UpdateSession::open();

  for_each_occurrence ( occurrence, getHyperNetRootNetOccurrences() ) {
    HyperNet  hyperNet ( occurrence );
    if ( !occurrence.getPath().isEmpty() ) {
      DeepNet* deepNet = DeepNet::create ( hyperNet );
      if (deepNet) deepNet->_createRoutingPads ( buildRings );
    } else {
      RoutingPad* previousRP = NULL;
      RoutingPad* currentRP  = NULL;
      Net* net = static_cast<Net*>(occurrence.getEntity());

      for_each_component ( component, net->getComponents() ) {
        Plug* primaryPlug = dynamic_cast<Plug*>( component );
        if ( primaryPlug ) {
          if ( !primaryPlug->getBodyHook()->getSlaveHooks().IsEmpty() ) {
            cerr << "[ERROR] " << primaryPlug << "\n"
                 << "        has attached components, not managed yet." << endl;
          } else {
            primaryPlug->getBodyHook()->detach ();
          }
        }
        end_for
      }

      for_each_occurrence ( plugOccurrence, hyperNet.getLeafPlugOccurrences() ) {
        currentRP = createRoutingPad ( net, plugOccurrence );
        currentRP->materialize ();
        if ( buildRings ) {
          if ( previousRP ) {
            currentRP->getBodyHook()->attach ( previousRP->getBodyHook() );
          }
          Plug* plug = static_cast<Plug*>( plugOccurrence.getEntity() );
          if ( plugOccurrence.getPath().isEmpty() ) {
            plug->getBodyHook()->attach ( currentRP->getBodyHook() );
            plug->getBodyHook()->detach ();
          }
          previousRP = currentRP;
        }

        end_for
      }

      for_each_component ( component, net->getComponents() ) {
        Pin* pin = dynamic_cast<Pin*>( component );
        if ( pin ) {
          currentRP = createRoutingPad ( pin );
          if ( buildRings ) {
            if ( previousRP ) {
              currentRP->getBodyHook()->attach ( previousRP->getBodyHook() );
            }
            pin->getBodyHook()->attach ( currentRP->getBodyHook() );
            pin->getBodyHook()->detach ();
          }
          previousRP = currentRP;
        }
        
        end_for
      }
    }
    end_for
  }

  UpdateSession::close();
}

void Cell::materialize()
// *********************
{
    for_each_instance(instance, getInstances()) instance->materialize(); end_for;
    for_each_net(net, getNets()) net->materialize(); end_for;
    for_each_marker(marker, getMarkers()) marker->materialize(); end_for;
}

void Cell::unmaterialize()
// ***********************
{
    for_each_instance(instance, getInstances()) instance->unmaterialize(); end_for;
    for_each_net(net, getNets()) net->unmaterialize(); end_for;
    for_each_marker(marker, getMarkers()) marker->unmaterialize(); end_for;
}

void Cell::_postCreate()
// *********************
{
    _library->_getCellMap()._insert(this);

    Inherit::_postCreate();
}

void Cell::_preDestroy()
// ********************
{
    Inherit::_preDestroy();

    while(_slaveEntityMap.size()) {
      _slaveEntityMap.begin()->second->destroy();
    }

    //for_each_view(view, getViews()) view->SetCell(NULL); end_for;
    for_each_marker(marker, getMarkers()) marker->destroy(); end_for;
    for_each_instance(slaveInstance, getSlaveInstances()) slaveInstance->destroy(); end_for;
    for_each_instance(instance, getInstances()) instance->destroy(); end_for;
    for_each_net(net, getNets()) net->destroy(); end_for;
    for_each_slice(slice, getSlices()) slice->_destroy(); end_for;

    _library->_getCellMap()._remove(this);
}

string Cell::_getString() const
// ****************************
{
    string s = Inherit::_getString();
    s.insert(s.length() - 1, " " + getString(_name));
    return s;
}

Record* Cell::_getRecord() const
// ***********************
{
    Record* record = Inherit::_getRecord();
    if (record) {
        record->add(getSlot("Library", _library));
        record->add(getSlot("Name", &_name));
        record->add(getSlot("Instances", &_instanceMap));
        record->add(getSlot("QuadTree", &_quadTree));
        record->add(getSlot("SlaveInstances", &_slaveInstanceSet));
        record->add(getSlot("Nets", &_netMap));
        record->add(getSlot("Pins", &_pinMap));
        record->add(getSlot("Slices", &_sliceMap));
        record->add(getSlot("Markers", &_markerSet));
        record->add(getSlot("AbutmentBox", &_abutmentBox));
        record->add(getSlot("BoundingBox", &_boundingBox));
        record->add(getSlot("isTerminal", &_isTerminal));
        record->add(getSlot("isFlattenLeaf", &_isFlattenLeaf));
    }
    return record;
}

void Cell::_fit(const Box& box)
// ****************************
{
    if (box.isEmpty()) return;
    if (_boundingBox.isEmpty()) return;
    if (_boundingBox.contains(box)) return;
    _boundingBox.merge(box);
    for_each_instance(instance, getSlaveInstances()) {
        instance->getCell()->_fit(instance->getTransformation().getBox(box));
        end_for;
    }
}

void Cell::_unfit(const Box& box)
// ******************************
{
    if (box.isEmpty()) return;
    if (_boundingBox.isEmpty()) return;
    if (!_boundingBox.isConstrainedBy(box)) return;
    _boundingBox.makeEmpty();
    for_each_instance(instance, getSlaveInstances()) {
        instance->getCell()->_unfit(instance->getTransformation().getBox(box));
        end_for;
    }
}

void Cell::_addSlaveEntity(Entity* entity, Entity* slaveEntity)
// ************************************************************************
{
  assert(entity->getCell() == this);

  _slaveEntityMap.insert(pair<Entity*,Entity*>(entity,slaveEntity));
}

void Cell::_removeSlaveEntity(Entity* entity, Entity* slaveEntity)
// ***************************************************************************
{
  assert(entity->getCell() == this);

  pair<SlaveEntityMap::iterator,SlaveEntityMap::iterator>
    bounds = _slaveEntityMap.equal_range(entity);
  multimap<Entity*,Entity*>::iterator it = bounds.first;
  for(; it != bounds.second ; it++ ) {
    if (it->second == slaveEntity) {
      _slaveEntityMap.erase(it);
      break;
    }
  }
}

void Cell::_getSlaveEntities(SlaveEntityMap::iterator& begin, SlaveEntityMap::iterator& end)
// *********************************************************************************************************
{
  begin = _slaveEntityMap.begin();
  end   = _slaveEntityMap.end();
}

void Cell::_getSlaveEntities(Entity* entity, SlaveEntityMap::iterator& begin, SlaveEntityMap::iterator& end)
// *********************************************************************************************************
{
  begin = _slaveEntityMap.lower_bound(entity);
  end   = _slaveEntityMap.upper_bound(entity);
}

// ****************************************************************************************************
// Cell::InstanceMap implementation
// ****************************************************************************************************

Cell::InstanceMap::InstanceMap()
// *****************************
:    Inherit()
{
}

Name Cell::InstanceMap::_getKey(Instance* instance) const
// ******************************************************
{
    return instance->getName();
}

unsigned Cell::InstanceMap::_getHashValue(Name name) const
// *******************************************************
{
    return ( (unsigned int)( (unsigned long)name._getSharedName() ) ) / 8;
}

Instance* Cell::InstanceMap::_getNextElement(Instance* instance) const
// *******************************************************************
{
    return instance->_getNextOfCellInstanceMap();
}

void Cell::InstanceMap::_setNextElement(Instance* instance, Instance* nextInstance) const
// **************************************************************************************
{
    instance->_setNextOfCellInstanceMap(nextInstance);
}



// ****************************************************************************************************
// Cell::SlaveInstanceSet implementation
// ****************************************************************************************************

Cell::SlaveInstanceSet::SlaveInstanceSet()
// ***************************************
:    Inherit()
{
}

unsigned Cell::SlaveInstanceSet::_getHashValue(Instance* slaveInstance) const
// **************************************************************************
{
    return ( (unsigned int)( (unsigned long)slaveInstance ) ) / 8;
}

Instance* Cell::SlaveInstanceSet::_getNextElement(Instance* slaveInstance) const
// *****************************************************************************
{
    return slaveInstance->_getNextOfCellSlaveInstanceSet();
}

void Cell::SlaveInstanceSet::_setNextElement(Instance* slaveInstance, Instance* nextSlaveInstance) const
// ****************************************************************************************************
{
    slaveInstance->_setNextOfCellSlaveInstanceSet(nextSlaveInstance);
}



// ****************************************************************************************************
// Cell::NetMap implementation
// ****************************************************************************************************

Cell::NetMap::NetMap()
// *******************
:    Inherit()
{
}

Name Cell::NetMap::_getKey(Net* net) const
// ***************************************
{
    return net->getName();
}

unsigned Cell::NetMap::_getHashValue(Name name) const
// **************************************************
{
    return ( (unsigned int)( (unsigned long)name._getSharedName() ) ) / 8;
}

Net* Cell::NetMap::_getNextElement(Net* net) const
// ***********************************************
{
    return net->_getNextOfCellNetMap();
}

void Cell::NetMap::_setNextElement(Net* net, Net* nextNet) const
// *************************************************************
{
    net->_setNextOfCellNetMap(nextNet);
}


// ****************************************************************************************************
// Cell::PinMap implementation
// ****************************************************************************************************

Cell::PinMap::PinMap()
// *******************
:    Inherit()
{
}

Name Cell::PinMap::_getKey(Pin* pin) const
// ***************************************
{
    return pin->getName();
}

unsigned Cell::PinMap::_getHashValue(Name name) const
// **************************************************
{
    return ( (unsigned int)( (unsigned long)name._getSharedName() ) ) / 8;
}

Pin* Cell::PinMap::_getNextElement(Pin* pin) const
// ***********************************************
{
    return pin->_getNextOfCellPinMap();
}

void Cell::PinMap::_setNextElement(Pin* pin, Pin* nextPin) const
// *************************************************************
{
    pin->_setNextOfCellPinMap(nextPin);
}


// ****************************************************************************************************
// Cell::SliceMap implementation
// ****************************************************************************************************

Cell::SliceMap::SliceMap()
// ***********************
:    Inherit()
{
}

const Layer* Cell::SliceMap::_getKey(Slice* slice) const
// *****************************************************
{
    return slice->getLayer();
}

unsigned Cell::SliceMap::_getHashValue(const Layer* layer) const
// *************************************************************
{
    return ( (unsigned int)( (unsigned long)layer ) ) / 8;
}

Slice* Cell::SliceMap::_getNextElement(Slice* slice) const
// *******************************************************
{
    return slice->_getNextOfCellSliceMap();
}

void Cell::SliceMap::_setNextElement(Slice* slice, Slice* nextSlice) const
// ***********************************************************************
{
    slice->_setNextOfCellSliceMap(nextSlice);
};



// ****************************************************************************************************
// Cell::MarkerSet implementation
// ****************************************************************************************************

Cell::MarkerSet::MarkerSet()
// *************************
:    Inherit()
{
}

unsigned Cell::MarkerSet::_getHashValue(Marker* marker) const
// **********************************************************
{
    return ( (unsigned int)( (unsigned long)marker ) ) / 8;
}

Marker* Cell::MarkerSet::_getNextElement(Marker* marker) const
// ***********************************************************
{
    return marker->_getNextOfCellMarkerSet();
}

void Cell::MarkerSet::_setNextElement(Marker* marker, Marker* nextMarker) const
// ****************************************************************************
{
    marker->_setNextOfCellMarkerSet(nextMarker);
}

} // End of Hurricane namespace.

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************
