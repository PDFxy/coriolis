
// -*- C++ -*-
//
// This file is part of the Coriolis Project.
// Copyright (C) Laboratoire LIP6 - Departement ASIM
// Universite Pierre et Marie Curie
//
// Main contributors :
//        Christophe Alexandre   <Christophe.Alexandre@lip6.fr>
//        Sophie Belloeil             <Sophie.Belloeil@lip6.fr>
//        Hugo Cl�ment                   <Hugo.Clement@lip6.fr>
//        Jean-Paul Chaput           <Jean-Paul.Chaput@lip6.fr>
//        Damien Dupuis                 <Damien.Dupuis@lip6.fr>
//        Christian Masson           <Christian.Masson@lip6.fr>
//        Marek Sroka                     <Marek.Sroka@lip6.fr>
// 
// The  Coriolis Project  is  free software;  you  can redistribute it
// and/or modify it under the  terms of the GNU General Public License
// as published by  the Free Software Foundation; either  version 2 of
// the License, or (at your option) any later version.
// 
// The  Coriolis Project is  distributed in  the hope that it  will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY  or FITNESS FOR  A PARTICULAR PURPOSE.   See the
// GNU General Public License for more details.
// 
// You should have  received a copy of the  GNU General Public License
// along with the Coriolis Project; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// License-Tag
// Authors-Tag
// ===================================================================
//
// $Id: PyCell.cpp,v 1.34 2008/02/07 17:09:41 xtof Exp $
//
// x-----------------------------------------------------------------x 
// |                                                                 |
// |                   C O R I O L I S                               |
// |    I s o b a r  -  Hurricane / Python Interface                 |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Module  :       "./PyCell.cpp"                             |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x

#include "hurricane/isobar/PyCell.h"
#include "hurricane/isobar/PyBox.h"
#include "hurricane/isobar/PyName.h"
#include "hurricane/isobar/PyLibrary.h"
#include "hurricane/isobar/PyInstance.h"
#include "hurricane/isobar/PyOccurrence.h"
#include "hurricane/isobar/ProxyProperty.h"
#include "hurricane/isobar/PyNet.h"
#include "hurricane/isobar/PyNetCollection.h"
#include "hurricane/isobar/PyReferenceCollection.h"
#include "hurricane/isobar/PyInstanceCollection.h"
#include "hurricane/isobar/PyOccurrenceCollection.h"

namespace  Isobar {


using namespace Hurricane;

extern "C" {


#undef  ACCESS_OBJECT
#undef  ACCESS_CLASS
#define ACCESS_OBJECT           _baseObject._object
#define ACCESS_CLASS(_pyObject)  &(_pyObject->_baseObject)
#define METHOD_HEAD(function)   GENERIC_METHOD_HEAD(Cell,cell,function)


// x=================================================================x
// |                "PyCell" Python Module Code Part                 |
// x=================================================================x

#if defined(__PYTHON_MODULE__)


  // x-------------------------------------------------------------x
  // |                "PyCell" Attribute Methods                   |
  // x-------------------------------------------------------------x
  

  // Standart Accessors (Attributes).


  // Standart Delete (Attribute).
  DBoDestroyAttribute(PyCell_destroy,PyCell)



  
  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getLibrary ()"

  static PyObject* PyCell_getLibrary ( PyCell *self ) {
    trace << "PyCell_getLibrary ()" << endl;

    Library* library = NULL;

    HTRY
    METHOD_HEAD ( "Cell.getLibrary()" )
    library = cell->getLibrary ();
    HCATCH
    
    return PyLibrary_Link(library);
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getName ()"

  static PyObject* PyCell_getName(PyCell *self) {
    trace << "PyCell_getName ()" << endl;
    
    METHOD_HEAD ( "Cell.getName()" )

    PyName* pyName = PyObject_NEW ( PyName, &PyTypeName );
    if ( pyName == NULL ) { return NULL; }

    HTRY
    pyName->_object = new Name ( cell->getName() );
    HCATCH
    
    return (PyObject*)pyName;
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getInstance ()"

  static PyObject* PyCell_getInstance ( PyCell *self, PyObject* args ) {
    trace << "PyCell_getInstance ()" << endl;

    Instance* instance = NULL;
    
    HTRY
    METHOD_HEAD ( "Cell.getInstance()" )

    PyName* arg0;
    if ( ! ParseOneArg ( "Cell.getInstance", args, NAME_ARG, (PyObject**)&arg0 ) )
        return NULL;

    instance = cell->getInstance ( *PYNAME_O(arg0) );
    HCATCH
    
    return PyInstance_Link ( instance );
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getInstances()"

  static PyObject* PyCell_getInstances( PyCell *self ) {
    trace << "PyCell_getInstances()" << endl;

    METHOD_HEAD ( "Cell.getInstances()" )

    PyInstanceCollection* pyInstanceCollection = NULL;

    HTRY
    Instances* instances = new Instances(cell->getInstances());

    pyInstanceCollection = PyObject_NEW(PyInstanceCollection, &PyTypeInstanceCollection);
    if (pyInstanceCollection == NULL) { 
        return NULL;
    }

    pyInstanceCollection->_object = instances;
    HCATCH
    
    return (PyObject*)pyInstanceCollection;
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getInstancesUnder()"

  static PyObject* PyCell_getInstancesUnder(PyCell *self, PyObject* args) {
    trace << "PyCell_getInstancesUnder()" << endl;

    METHOD_HEAD("Cell.getInstancesUnder()")

    PyBox* arg0;
    if (!ParseOneArg("Cell.getInstancesUnder", args, BOX_ARG, (PyObject**)&arg0))
        return NULL;        

    PyInstanceCollection* pyInstanceCollection = NULL;

    HTRY
    Instances* instances = new Instances(cell->getInstancesUnder(*PYBOX_O(arg0)));

    pyInstanceCollection = PyObject_NEW(PyInstanceCollection, &PyTypeInstanceCollection);
    if (pyInstanceCollection == NULL) { 
        return NULL;
    }

    pyInstanceCollection->_object = instances;
    HCATCH
    
    return (PyObject*)pyInstanceCollection;
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getSlaveInstances()"

  static PyObject* PyCell_getSlaveInstances(PyCell *self) {
    trace << "PyCell_getSlaveInstances()" << endl;

    METHOD_HEAD("Cell.getSlaveInstances()")

    PyInstanceCollection* pyInstanceCollection = NULL;

    HTRY
    Instances* instances = new Instances(cell->getSlaveInstances());

    pyInstanceCollection = PyObject_NEW(PyInstanceCollection, &PyTypeInstanceCollection);
    if (pyInstanceCollection == NULL) { 
        return NULL;
    }

    pyInstanceCollection->_object = instances;
    HCATCH
    
    return (PyObject*)pyInstanceCollection;
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getOccurrences()"

  static PyObject* PyCell_getOccurrences(PyCell *self) {
    trace << "PyCell_getOccurrences()" << endl;

    METHOD_HEAD("Cell.getOccurrences()")

    PyOccurrenceCollection* pyOccurrenceCollection = NULL;

    HTRY
    Occurrences* occurrences = new Occurrences(cell->getOccurrences());

    pyOccurrenceCollection = PyObject_NEW(PyOccurrenceCollection, &PyTypeOccurrenceCollection);
    if (pyOccurrenceCollection == NULL) { 
        return NULL;
    }

    pyOccurrenceCollection->_object = occurrences;
    HCATCH
    
    return (PyObject*)pyOccurrenceCollection;
  }



  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getOccurrencesUnder()"

  static PyObject* PyCell_getOccurrencesUnder(PyCell *self, PyObject* args) {
    trace << "PyCell_getOccurrencesUnder()" << endl;

    METHOD_HEAD("Cell.getOccurrencesUnder()")

    PyBox* arg0;
    if (!ParseOneArg("Cell.getOccurrencesUnder", args, BOX_ARG, (PyObject**)&arg0))
        return NULL;

    PyOccurrenceCollection* pyOccurrenceCollection = NULL;

    HTRY
    Occurrences* occurrences = new Occurrences(cell->getOccurrencesUnder(*PYBOX_O(arg0)));

    pyOccurrenceCollection = PyObject_NEW(PyOccurrenceCollection, &PyTypeOccurrenceCollection);
    if (pyOccurrenceCollection == NULL) { 
        return NULL;
    }

    pyOccurrenceCollection->_object = occurrences;
    HCATCH
    
    return (PyObject*)pyOccurrenceCollection;
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getLeafInstanceOccurrences()"

  static PyObject* PyCell_getLeafInstanceOccurrences(PyCell *self) {
    trace << "PyCell_getLeafInstanceOccurrences()" << endl;

    METHOD_HEAD ( "Cell.getLeafInstanceOccurrences()" )

    PyOccurrenceCollection* pyOccurrenceCollection = NULL;

    HTRY
    Occurrences* occurrences = new Occurrences(cell->getLeafInstanceOccurrences());

    pyOccurrenceCollection = PyObject_NEW(PyOccurrenceCollection, &PyTypeOccurrenceCollection);
    if (pyOccurrenceCollection == NULL) { 
        return NULL;
    }

    pyOccurrenceCollection->_object = occurrences;
    HCATCH
    
    return (PyObject*)pyOccurrenceCollection;
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getLeafInstanceOccurrencesUnder()"

  static PyObject* PyCell_getLeafInstanceOccurrencesUnder(PyCell *self, PyObject* args) {
    trace << "PyCell_getLeafInstanceOccurrencesUnder()" << endl;

    METHOD_HEAD ( "Cell.getLeafInstanceOccurrencesUnder()" )

    PyBox* arg0;
    if (!ParseOneArg("Cell.getLeafInstanceOccurencesUnder", args, BOX_ARG, (PyObject**)&arg0))
        return NULL;        

    PyOccurrenceCollection* pyOccurrenceCollection = NULL;

    HTRY
    Occurrences* occurrences = new Occurrences(cell->getLeafInstanceOccurrencesUnder(*PYBOX_O(arg0)));

    pyOccurrenceCollection = PyObject_NEW(PyOccurrenceCollection, &PyTypeOccurrenceCollection);
    if (pyOccurrenceCollection == NULL) { 
        return NULL;
    }

    pyOccurrenceCollection->_object = occurrences;
    HCATCH
    
    return (PyObject*)pyOccurrenceCollection;
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getReferences()"

  static PyObject* PyCell_getReferences(PyCell *self) {
    trace << "PyCell_getReferences()" << endl;

    METHOD_HEAD("Cell.getReferences()")

    PyReferenceCollection* pyReferenceCollection = NULL;

    HTRY
    References* references = new References(cell->getReferences());

    pyReferenceCollection = PyObject_NEW(PyReferenceCollection, &PyTypeReferenceCollection);
    if (pyReferenceCollection == NULL) { 
        return NULL;
    }

    pyReferenceCollection->_object = references;
    HCATCH
    
    return (PyObject*)pyReferenceCollection;
  }

  
  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getHyperNets()"

  static PyObject* PyCell_getHyperNets(PyCell *self) {
    trace << "PyCell_getHyperNets()" << endl;

    METHOD_HEAD ( "Cell.getHyperNets()" )

    PyOccurrenceCollection* pyOccurrenceCollection = NULL;

    HTRY
    Occurrences* occurrences = new Occurrences(cell->getHyperNetRootNetOccurrences());

    pyOccurrenceCollection = PyObject_NEW(PyOccurrenceCollection, &PyTypeOccurrenceCollection);
    if (pyOccurrenceCollection == NULL) { 
        return NULL;
    }

    pyOccurrenceCollection->_object = occurrences;
    HCATCH
    
    return ( (PyObject*)pyOccurrenceCollection);
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getNet ()"

  static PyObject* PyCell_getNet ( PyCell *self, PyObject* args ) {
    trace << "PyCell_getNet ()" << endl;

    Net* net = NULL;

    HTRY
    METHOD_HEAD ( "Cell.getNet()" )

    PyName* arg0;
    if ( ! ParseOneArg ( "Cell.getNet", args, NAME_ARG, (PyObject**)&arg0 ) )
        return NULL;
    
    net = cell->getNet ( *PYNAME_O(arg0) );
    HCATCH
    
    return PyNet_Link ( net );
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getNets ()"

  static PyObject* PyCell_getNets ( PyCell *self ) {
    trace << "PyCell_getNets()" << endl;

    METHOD_HEAD("Cell.getNets()")

    PyNetCollection* pyNetCollection = NULL;

    HTRY
    Nets* nets = new Nets(cell->getNets());

    pyNetCollection = PyObject_NEW(PyNetCollection, &PyTypeNetCollection);
    if (pyNetCollection == NULL) { 
        return NULL;
    }

    pyNetCollection->_object = nets;
    HCATCH
    
    return ( (PyObject*)pyNetCollection);
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getExternalNets()"

  static PyObject* PyCell_getExternalNets(PyCell *self) {
    trace << "PyCell_getExternalNets()" << endl;

    METHOD_HEAD("Cell.getExternalNets()")

    PyNetCollection* pyNetCollection = NULL;

    HTRY
    Nets* nets = new Nets(cell->getExternalNets());

    pyNetCollection = PyObject_NEW(PyNetCollection, &PyTypeNetCollection);
    if (pyNetCollection == NULL) { 
        return NULL;
    }
    pyNetCollection->_object = nets;

    HCATCH

    return ((PyObject*)pyNetCollection);
  }  

  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getClockNets()"

  static PyObject* PyCell_getClockNets(PyCell *self) {
    trace << "PyCell_getClockNets()" << endl;

    METHOD_HEAD("Cell.getClockNets")

    PyNetCollection* pyNetCollection = NULL;

    HTRY
    Nets* nets = new Nets(cell->getClockNets());

    pyNetCollection = PyObject_NEW(PyNetCollection, &PyTypeNetCollection);
    if (pyNetCollection == NULL) { 
        return NULL;
    }
    pyNetCollection->_object = nets;

    HCATCH

    return ((PyObject*)pyNetCollection);
  }

  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getSupplyNets()"

  static PyObject* PyCell_getSupplyNets(PyCell *self) {
    trace << "PyCell_getSupplyNets()" << endl;

    METHOD_HEAD ( "Cell.getSupplyNets()" )

    PyNetCollection* pyNetCollection = NULL;

    HTRY
    Nets* nets = new Nets(cell->getSupplyNets());

    pyNetCollection = PyObject_NEW(PyNetCollection, &PyTypeNetCollection);
    if (pyNetCollection == NULL) { 
        return NULL;
    }
    pyNetCollection->_object = nets;

    HCATCH

    return ((PyObject*)pyNetCollection);
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getPowerNets()"

  static PyObject* PyCell_getPowerNets(PyCell *self) {
    trace << "PyCell_getPowerNets()" << endl;

    METHOD_HEAD ( "Cell.getPowerNets()" )

    PyNetCollection* pyNetCollection = NULL;

    HTRY
    Nets* nets = new Nets(cell->getPowerNets());

    pyNetCollection = PyObject_NEW(PyNetCollection, &PyTypeNetCollection);
    if (pyNetCollection == NULL) { 
        return NULL;
    }
    pyNetCollection->_object = nets;

    HCATCH

    return ((PyObject*)pyNetCollection);
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getGroundNets()"

  static PyObject* PyCell_getGroundNets(PyCell *self) {
    trace << "PyCell_getGroundNets()" << endl;

    METHOD_HEAD ( "Cell.getGroundNets()" )

    PyNetCollection* pyNetCollection = NULL;

    HTRY
    Nets* nets = new Nets(cell->getGroundNets());

    pyNetCollection = PyObject_NEW(PyNetCollection, &PyTypeNetCollection);
    if (pyNetCollection == NULL) { 
        return NULL;
    }
    pyNetCollection->_object = nets;

    HCATCH

    return ((PyObject*)pyNetCollection);
  }

  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_getAbutmentBox ()"

  static PyObject* PyCell_getAbutmentBox ( PyCell *self ) {
    trace << "PyCell_getAbutmentBox()" << endl;
    
    METHOD_HEAD ( "Cell.getAbutmentBox()" )

    PyBox* abutmentBox = PyObject_NEW ( PyBox, &PyTypeBox );
    if (abutmentBox == NULL) { return NULL; }

    HTRY
    abutmentBox->_object = new Box ( cell->getAbutmentBox() );
    HCATCH

    return ( (PyObject*)abutmentBox );
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_setName ()"

  static PyObject* PyCell_setName ( PyCell *self, PyObject* args )
  {
    trace << "Cell.setName()" << endl;

    HTRY
    METHOD_HEAD ( "Cell.setName()" )

    PyName* name;
    if ( ! ParseOneArg ( "Cell.setName", args, NAME_ARG, (PyObject**)&name ) )
        return NULL;
    cell->setName ( *PYNAME_O(name) );
    HCATCH

    Py_RETURN_NONE;
  }


  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_setAbutmentBox ()"

  static PyObject* PyCell_setAbutmentBox ( PyCell *self, PyObject* args ) {
    trace << "Cell.setAbutmentBox()" << endl;

    HTRY
    METHOD_HEAD ( "Cell.setAbutmentBox()" )

    PyBox* abutmentBox;
    if ( ! ParseOneArg ( "Cell.setAbutmentBox", args, BOX_ARG, (PyObject**)&abutmentBox ) )
        return NULL;
    cell->setAbutmentBox ( *PYBOX_O(abutmentBox) );
    HCATCH

    Py_RETURN_NONE;
  }
  
  
  // ---------------------------------------------------------------
  // Attribute Method  :  "PyCell_setTerminal ()"

  static PyObject* PyCell_setTerminal ( PyCell *self, PyObject* args ) {
    trace << "PyCell_setTerminal ()" << endl;

    HTRY
    METHOD_HEAD ( "Cell.setTerminal()" )

    PyObject* arg0;
    if ( ! ParseOneArg ( "Cell.setTerminal", args, INT_ARG, (PyObject**)&arg0 ) )
        return NULL;
    cell->setTerminal ( PyInt_AsLong(arg0) != 0 );
    HCATCH

    Py_RETURN_NONE;
  }


  // Standart Predicates (Attributes).
  DirectGetBoolAttribute(PyCell_isTerminal, isTerminal ,PyCell,Cell)
  DirectGetBoolAttribute(PyCell_isLeaf,         isLeaf ,PyCell,Cell)

  GetBoundStateAttribute(PyCell_isPyBound              ,PyCell,Cell)

  // ---------------------------------------------------------------
  // PyCell Attribute Method table.

  PyMethodDef PyCell_Methods[] =
    { { "getLibrary"          , (PyCFunction)PyCell_getLibrary           , METH_NOARGS , "Returns the library owning the cell." }
    , { "getName"             , (PyCFunction)PyCell_getName              , METH_NOARGS , "Returns the name of the cell." }
    , { "getInstance"         , (PyCFunction)PyCell_getInstance          , METH_VARARGS, "Returns the instance of name <name> if it exists, else NULL." }
    , { "getInstances", (PyCFunction)PyCell_getInstances, METH_NOARGS , "Returns the locator of the collection of all instances called by the cell." } // getInstances
    , { "getInstancesUnder"  , (PyCFunction)PyCell_getInstancesUnder  , METH_VARARGS, "Returns the locator of the collection of all instances of the cell intersecting the given rectangular area." } // getInstancesUnder
    , { "getSlaveInstances"  , (PyCFunction)PyCell_getSlaveInstances  , METH_NOARGS , "Returns the locator of the collection of instances whose master is this cell." } // getSlaveInstances
    , { "getOccurrences"     , (PyCFunction)PyCell_getOccurrences     , METH_VARARGS, "Returns the collection of all occurrences belonging to the cell." }
    , { "getOccurrencesUnder", (PyCFunction)PyCell_getOccurrencesUnder, METH_NOARGS , "Returns the collection of all occurrences belonging to this cell and intersecting the given rectangular area." }
    , { "getLeafInstanceOccurrences", (PyCFunction)PyCell_getLeafInstanceOccurrences, METH_VARARGS, "Returns the collection of all occurrences belonging to the cell." }
    , { "getLeafInstanceOccurrencesUnder", (PyCFunction)PyCell_getLeafInstanceOccurrencesUnder, METH_NOARGS , "Returns the collection of all occurrences belonging to this cell and intersecting the given rectangular area." }
    , { "getReferences"      , (PyCFunction)PyCell_getReferences      , METH_VARARGS, "Returns the collection of all references belonging to the cell." }
    , { "getHyperNets"       , (PyCFunction)PyCell_getHyperNets       , METH_VARARGS, "Returns the collection of all hyperNets belonging to the cell." }
    , { "getNet"              , (PyCFunction)PyCell_getNet              , METH_VARARGS, "Returns the net of name <name> if it exists, else NULL." }
    , { "getNets"              , (PyCFunction)PyCell_getNets      , METH_NOARGS , "Returns the collection of all nets of the cell." }
    , { "getExternalNets", (PyCFunction)PyCell_getExternalNets, METH_NOARGS , "Returns the collection of all external nets of the cell." }
    , { "getClockNets" , (PyCFunction)PyCell_getClockNets, METH_NOARGS , "Returns the collection of all clock nets of the cell." }
    , { "getSupplyNets", (PyCFunction)PyCell_getSupplyNets, METH_NOARGS , "Returns the collection of all supply nets of the cell." }
    , { "getPowerNets" , (PyCFunction)PyCell_getPowerNets, METH_NOARGS , "Returns the collection of all power nets of the cell." }
    , { "getGroundNets", (PyCFunction)PyCell_getGroundNets, METH_NOARGS , "Returns the collection of all ground nets of the cell." }
    , { "getAbutmentBox"      , (PyCFunction)PyCell_getAbutmentBox      , METH_NOARGS , "Returns the abutment box of the cell(which is defined by the designer unlike the bounding box which is managed dynamically)" }
    , { "isTerminal"          , (PyCFunction)PyCell_isTerminal          , METH_NOARGS , "Returns true if the cell is marked as terminal, else false." }
    , { "isLeaf"              , (PyCFunction)PyCell_isLeaf              , METH_NOARGS , "Returns true if the cell is a leaf of the hierarchy, else false." }
    , { "isBound"             , (PyCFunction)PyCell_isPyBound           , METH_NOARGS, "Returns true if the cell is bounded to the hurricane cell" }    
    , { "setName"             , (PyCFunction)PyCell_setName             , METH_VARARGS, "Allows to change the cell name." }
    , { "setAbutmentBox"      , (PyCFunction)PyCell_setAbutmentBox      , METH_VARARGS, "Sets the cell abutment box." }
    , { "setTerminal"         , (PyCFunction)PyCell_setTerminal         , METH_VARARGS, "Sets the cell terminal status." }
    , { "destroy"             , (PyCFunction)PyCell_destroy             , METH_NOARGS
                              , "Destroy associated hurricane object The python object remains." }
    , {NULL, NULL, 0, NULL}           /* sentinel */
    };


  // x-------------------------------------------------------------x
  // |                  "PyCell" Object Methods                    |
  // x-------------------------------------------------------------x

  static PyObject* PyCell_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    trace << "PyCell_new()" << endl;

    PyObject* arg0;
    PyObject* arg1;
    string lib_name_arg = ":library:name";
    if (!ParseTwoArg("Cell.init", args, lib_name_arg, &arg0, &arg1)) {
        PyErr_SetString ( ConstructorError, "invalid number of parameters for Cell constructor.");
        return NULL;
    }

    Cell* cell = NULL;

    HTRY
    cell = Cell::create(PYLIBRARY_O(arg0), getString(*PYNAME_O(arg1)));
    HCATCH

    return PyCell_Link(cell);
  }

  DBoDeleteMethod(Cell)
  PyTypeObjectLinkPyType(Cell)
  PyTypeObjectConstructor(Cell)

#else  // End of Python Module Code Part.


// x=================================================================x
// |               "PyCell" Shared Library Code Part                 |
// x=================================================================x


  // Link/Creation Method.
  DBoLinkCreateMethod(Cell)


  // ---------------------------------------------------------------
  // PyCell Object Definitions.
  PyTypeObjectDefinitions(Cell)


#endif  // End of Shared Library Code Part.


}  // End of extern "C".




}  // End of Isobar namespace.