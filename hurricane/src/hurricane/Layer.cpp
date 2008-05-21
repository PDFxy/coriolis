// ****************************************************************************************************
// File: Layer.cpp
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#include "hurricane/Layer.h"
#include "hurricane/Technology.h"
#include "hurricane/Error.h"

namespace Hurricane {



// ****************************************************************************************************
// Layer implementation
// ****************************************************************************************************

Layer::Layer(Technology* technology, const Name& name, const Unit& minimalSize, const Unit& minimalSpacing, const Unit& pitch)
// ****************************************************************************************************
:    Inherit(),
    _technology(technology),
    _name(name),
    _mask(0),
    _extractMask(0),
    _minimalSize(minimalSize),
    _minimalSpacing(minimalSpacing),
    _pitch(pitch),
    _nextOfTechnologyLayerMap(NULL)
{
    if (!_technology)
        throw Error("Can't create " + _TName("Layer") + " : null technology");

    if (_name.isEmpty())
        throw Error("Can't create " + _TName("Layer") + " : empty name");

    if (_technology->getLayer(_name))
        throw Error("Can't create " + _TName("Layer") + " " + getString(_name) + " : already exists");
}

bool Layer::contains(const Layer* layer) const
// *******************************************
{
    return (layer && ((_mask & layer->getMask()) == layer->getMask()));
}

bool Layer::intersect(const Layer* layer) const
// ********************************************
{
    return ((_mask & layer->getMask()) != 0);
}

void Layer::setName(const Name& name)
// **********************************
{
    if (name != _name) {
        if (name.isEmpty())
            throw Error("Can't change layer name : empty name");

        if (_technology->getLayer(name))
            throw Error("Can't change layer name : already exists");

        _technology->_getLayerMap()._remove(this);
        _name = name;
        _technology->_getLayerMap()._insert(this);
    }
}

void Layer::setMinimalSize(const Unit& minimalSize)
// ************************************************
{
    _minimalSize = minimalSize;
}

void Layer::setMinimalSpacing(const Unit& minimalSpacing)
// ******************************************************
{
    _minimalSpacing = minimalSpacing;
}

void Layer::setPitch(const Unit& pitch)
// ************************************
{
    _pitch = pitch;
}

void Layer::_postCreate()
// **********************
{
    _technology->_getLayerMap()._insert(this);
    _technology->_getLayerList().push_back(this);

    Inherit::_postCreate();
}

void Layer::_preDestroy()
// *********************
{
    Inherit::_preDestroy();

    _technology->_getLayerList().remove(this);
    _technology->_getLayerMap()._remove(this);
}

string Layer::_getString() const
// *****************************
{
    string s = Inherit::_getString();
    s.insert(s.length() - 1, " " + getString(_name));
    return s;
}

Record* Layer::_getRecord() const
// ************************
{
    Record* record = Inherit::_getRecord();
    if (record) {
        record->add(getSlot("Technology", _technology));
        record->add(getSlot("Name", &_name));
        record->add(getSlot("Mask", &_mask));
        record->add(getSlot("ExtractMask", &_extractMask));
        record->add(getSlot("MinimalSize", &_minimalSize));
        record->add(getSlot("MinimalSpacing", &_minimalSpacing));
        record->add(getSlot("Pitch", &_pitch));
    }
    return record;
}

} // End of Hurricane namespace.

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************
