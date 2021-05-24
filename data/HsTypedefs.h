#pragma once

///\file HsTypedefs.h
/// Handful of usable typedefs for the main data classes

#include <QVector>

class HolyService;
class Client;
class Priest;
class Church;
class IServiceFilter;

typedef QVector<const HolyService *> tConstHolyServiceVector;
typedef QVector<HolyService *> tHolyServiceVector;
typedef tHolyServiceVector::iterator tHolyServiceVectorIt;
typedef tHolyServiceVector::const_iterator tHolyServiceVectorConstIt;

typedef QVector<Client *> tClientVector;
typedef tClientVector::iterator tClientVectorIt;
typedef tClientVector::const_iterator tClientVectorConstIt;

typedef QVector<Priest *> tPriestVector;
typedef tPriestVector::iterator tPriestVectorIt;
typedef tPriestVector::const_iterator tPriestVectorConstIt;

typedef QVector<Church *> tChurchVector;
typedef tChurchVector::iterator tChurchVectorIt;
typedef tChurchVector::const_iterator tChurchVectorConstIt;

typedef QVector<IServiceFilter *> tServiceFilterVector;
typedef tServiceFilterVector::iterator tServiceFilterVectorIt;
typedef tServiceFilterVector::const_iterator tServiceFilterVectorConstIt;


