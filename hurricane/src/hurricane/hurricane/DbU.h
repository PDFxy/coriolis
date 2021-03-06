// -*- C++ -*-
//
// Copyright (c) BULL S.A. 2000-2015, All Rights Reserved
//
// This file is part of Hurricane.
//
// Hurricane is free software: you can redistribute it  and/or  modify
// it under the terms of the GNU  Lesser  General  Public  License  as
// published by the Free Software Foundation, either version 3 of  the
// License, or (at your option) any later version.
//
// Hurricane is distributed in the hope that it will  be  useful,  but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHAN-
// TABILITY or FITNESS FOR A PARTICULAR PURPOSE. See  the  Lesser  GNU
// General Public License for more details.
//
// You should have received a copy of the Lesser  GNU  General  Public
// License along with Hurricane. If not, see
//                                     <http://www.gnu.org/licenses/>.
//
// +-----------------------------------------------------------------+
// |                  H U R R I C A N E                              |
// |     V L S I   B a c k e n d   D a t a - B a s e                 |
// |                                                                 |
// |  Author      :                       Remy Escassut              |
// |  E-mail      :            Jean-Paul.Chaput@lip6.fr              |
// | =============================================================== |
// |  C++ Header  :  "./hurricane/DbU.h"                             |
// +-----------------------------------------------------------------+


#ifndef  HURRICANE_DBU_H
#define  HURRICANE_DBU_H


#include  <cmath>
#include  "hurricane/Commons.h"


namespace Hurricane {


  class DbU {
    public:
      enum UnitPower { Pico = 1
                     , Nano
                     , Micro
                     , Milli
                     , Unity
                     , Kilo
                     };
    public:
      typedef  long  Unit;
    public:
      enum  StringMode { Db            = (1<<0)
                       , Grid          = (1<<1)
                       , Symbolic      = (1<<2)
                       , Physical      = (1<<3)
                       , SmartTruncate = (1<<4)
                       };
      enum  SnapMode   { Inferior      = 1
                       , Superior      = 2
                       , Nearest       = 4
                       };

    public:
      static        void                checkGridBound          ( double value );
      static        void                checkLambdaBound        ( double value );
      static        void                checkPhysicalBound      ( double value, UnitPower p );
    // User to DB Converters.
      static inline Unit                fromDb                  ( long value );
      static inline Unit                fromGrid                ( double value );
      static inline Unit                fromLambda              ( double value );
      static inline Unit                fromPhysical            ( double value, UnitPower p );
    // Old naming scheme (was not very clear).
      static inline Unit                db                      ( long value );
      static inline Unit                grid                    ( double value );
      static inline Unit                lambda                  ( double value );
      static inline Unit                physicalToDbu           ( double value, UnitPower p );
    // Precision & Resolution Managment.                        
      static        unsigned int        getPrecision            ();
      static        unsigned int        getMaximalPrecision     ();
      static        double              getResolution           ();
      static        void                setPrecision            ( unsigned int precision );
    // Founder Grid Managment.                                  
      static        double              getUnitPower            ( UnitPower p );
      static        void                setPhysicalsPerGrid     ( double gridsPerLambda, UnitPower p );
      static        double              getPhysicalsPerGrid     ();
      static        double              physicalToGrid          ( double physical, UnitPower p );
    // Lamba Managment.                                         
      static        void                setGridsPerLambda       ( double gridsPerLambda );
      static        double              getGridsPerLambda       ();
    // Snap Grid Managment.
      static        DbU::Unit           getRealSnapGridStep     ();
      static        DbU::Unit           getOnRealSnapGrid       ( DbU::Unit u, SnapMode mode=Nearest );
      static inline void                setRealSnapGridStep     ( DbU::Unit step );
      static        DbU::Unit           getSymbolicSnapGridStep ();
      static        DbU::Unit           getOnSymbolicSnapGrid   ( DbU::Unit u, SnapMode mode=Nearest );
      static inline void                setSymbolicSnapGridStep ( DbU::Unit step );
      static        DbU::Unit           getOnCustomGrid         ( DbU::Unit u, DbU::Unit step, SnapMode mode=Nearest );
      static inline DbU::Unit           getOnPhysicalGrid       ( DbU::Unit u, SnapMode mode=Superior );
    // Conversions.
      static inline long                toDb                    ( Unit u );
      static inline double              toGrid                  ( Unit u );
      static inline double              toGrid                  ( double u );
      static inline double              toLambda                ( Unit u );
      static inline double              toLambda                ( double u );
      static inline double              toPhysical              ( Unit u, UnitPower p );
      static inline double              toPhysical              ( double u, UnitPower p );
    // Old naming scheme (not very clear).
      static inline long                getDb                   ( Unit u );
      static inline double              getGrid                 ( Unit u );
      static inline double              getGrid                 ( double u );
      static inline double              getLambda               ( Unit u );
      static inline double              getLambda               ( double u );
      static inline double              getPhysical             ( Unit u, UnitPower p );
      static inline double              getPhysical             ( double u, UnitPower p );
      static        string              getValueString          ( Unit u, int mode=SmartTruncate );
      static        string              getValueString          ( double u, int mode=SmartTruncate );
      static        Record*             getValueRecord          ( const Unit* u );
      static        Slot*               getValueSlot            ( const string& name, const Unit* u );
      static        void                setStringMode           ( unsigned int mode, UnitPower p=Nano );
      static        void                getStringMode           ( unsigned int& mode, UnitPower& p );
    private:
      static        void                _updateBounds           ();

    public:
    // Static Attributes: constants.
      static const Unit          Min;
      static const Unit          Max;
    private:
    // Internal: Static Attributes.
      static const unsigned int  _maximalPrecision;
      static unsigned int        _precision;
      static double              _resolution;
      static double              _gridsPerLambda;
      static double              _physicalsPerGrid;
      static unsigned int        _stringMode;
      static DbU::UnitPower      _stringModeUnitPower;
      static DbU::Unit           _realSnapGridStep;
      static DbU::Unit           _symbolicSnapGridStep;
      static double              _gridMax;
      static double              _lambdaMax;
      static double              _physicalMax;
  };


// Inline Functions.
// New converter naming scheme.
  inline DbU::Unit  DbU::fromDb                  ( long value )                 { return value; }
  inline DbU::Unit  DbU::fromGrid                ( double value )               { checkGridBound    (value);   return (long)rint( value/_resolution ); }
  inline DbU::Unit  DbU::fromLambda              ( double value )               { checkLambdaBound  (value);   return fromGrid(value*_gridsPerLambda); }
  inline DbU::Unit  DbU::fromPhysical            ( double value, UnitPower p )  { checkPhysicalBound(value,p); return fromGrid((value*getUnitPower(p))/_physicalsPerGrid); }
  inline long       DbU::toDb                    ( DbU::Unit u )                { return u; }
  inline double     DbU::toGrid                  ( DbU::Unit u )                { return _resolution*(double)u; }
  inline double     DbU::toGrid                  ( double u )                   { return _resolution*u; }
  inline double     DbU::toLambda                ( DbU::Unit u )                { return toGrid(u)/_gridsPerLambda; }
  inline double     DbU::toLambda                ( double u )                   { return toGrid(u)/_gridsPerLambda; }
  inline double     DbU::toPhysical              ( DbU::Unit u, UnitPower p )   { return (_physicalsPerGrid*_resolution*(double)u)/getUnitPower(p); }
  inline double     DbU::toPhysical              ( double u, UnitPower p )      { return (_physicalsPerGrid*_resolution*u)/getUnitPower(p); }

// Old converter naming scheme.
  inline DbU::Unit  DbU::db                      ( long value )                 { return fromDb(value); }
  inline DbU::Unit  DbU::grid                    ( double value )               { return fromGrid(value); }
  inline DbU::Unit  DbU::lambda                  ( double value )               { return fromLambda(value); }
  inline DbU::Unit  DbU::physicalToDbu           ( double value, UnitPower p )  { return fromPhysical(value,p); }
  inline long       DbU::getDb                   ( DbU::Unit u )                { return toDb(u); }
  inline double     DbU::getGrid                 ( DbU::Unit u )                { return toGrid(u); }
  inline double     DbU::getGrid                 ( double u )                   { return toGrid(u); }
  inline double     DbU::getLambda               ( DbU::Unit u )                { return toLambda(u); }
  inline double     DbU::getLambda               ( double u )                   { return toLambda(u); }
  inline double     DbU::getPhysical             ( DbU::Unit u, UnitPower p )   { return toPhysical(u,p); }
  inline double     DbU::getPhysical             ( double u, UnitPower p )      { return toPhysical(u,p); }

  inline void       DbU::setRealSnapGridStep     ( DbU::Unit step )             { _realSnapGridStep = step; }
  inline void       DbU::setSymbolicSnapGridStep ( DbU::Unit step )             { _symbolicSnapGridStep = step; }
  inline DbU::Unit  DbU::getOnPhysicalGrid       ( DbU::Unit u, SnapMode mode ) { return getOnCustomGrid(u, grid(1), mode); }


} // End of Hurricane namespace.


#endif  // HURRICANE_DBU_H
