/*
 *
 *  Copyright (C) 2003-2015, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose:
 *    class DcmTransferSyntaxMap
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dccftsmp.h" /* for class DcmTransferSyntaxMap */
#include "dcmtk/dcmdata/dcerror.h"  /* for EC_IllegalCall */

DcmTransferSyntaxMap::DcmTransferSyntaxMap()
: map_()
{
}

DcmTransferSyntaxMap::~DcmTransferSyntaxMap()
{
  clear();
}


DcmTransferSyntaxMap::DcmTransferSyntaxMap(const DcmTransferSyntaxMap& arg)
{
  /* Copy all map entries */
  OFMap<OFString, DcmTransferSyntaxList *>::const_iterator first = arg.map_.begin();
  OFMap<OFString, DcmTransferSyntaxList *>::const_iterator last = arg.map_.end();
  while (first != last)
  {
    DcmTransferSyntaxList* copy = new DcmTransferSyntaxList( *(*first).second );
    map_.insert( OFPair<const OFString, DcmTransferSyntaxList*>( (*first).first, copy ) );
    ++first;
  }
}


DcmTransferSyntaxMap& DcmTransferSyntaxMap::operator=(const DcmTransferSyntaxMap& arg)
{
  if (this != &arg)
  {
    /* Clear old entries and copy all map entries */
    this->clear();
    OFMap<OFString, DcmTransferSyntaxList *>::const_iterator first = arg.map_.begin();
    OFMap<OFString, DcmTransferSyntaxList *>::const_iterator last = arg.map_.end();
    while (first != last)
    {
      DcmTransferSyntaxList* copy = new DcmTransferSyntaxList( *(*first).second );
      map_.insert(OFPair<const OFString, DcmTransferSyntaxList*>( (*first).first, copy ) );
      ++first;
    }
  }
  return *this;
}

OFMap<OFString, DcmTransferSyntaxList*>::const_iterator DcmTransferSyntaxMap::begin()
{
  return map_.begin();
}

OFMap<OFString, DcmTransferSyntaxList*>::const_iterator DcmTransferSyntaxMap::end()
{
  return map_.end();
}

const DcmTransferSyntaxList* DcmTransferSyntaxMap::getTSList(const OFString& ts)
{
  OFMap<OFString, DcmTransferSyntaxList*>::const_iterator it = map_.begin();
  if ( it == map_.end() )
  {
    return NULL;
  }
  return (*it).second;
}

void DcmTransferSyntaxMap::clear()
{
  while (map_.size () != 0)
  {
    OFMap<OFString, DcmTransferSyntaxList *>::iterator first = map_.begin();
    delete (*first).second;
    map_.erase(first);
  }
}


size_t DcmTransferSyntaxMap::size() const
{
  return map_.size();
}

OFCondition DcmTransferSyntaxMap::add(
  const char *key,
  const char *transferSyntaxUID)
{
  if ((!key)||(!transferSyntaxUID)) return EC_IllegalCall;

  // perform syntax check of UID
  DcmUIDHandler uid(transferSyntaxUID);
  if (! uid.isValidUID())
  {
    OFString s("not a valid transfer syntax UID: ");
    s += transferSyntaxUID;
    return makeOFCondition(OFM_dcmnet, 1024, OF_error, s.c_str());
  }

  DcmTransferSyntaxList * const *value = NULL;
  OFString skey(key);
  OFMap<OFString, DcmTransferSyntaxList*>::iterator it = map_.find(skey);

  if (it == map_.end())
  {
    DcmTransferSyntaxList *newentry = new DcmTransferSyntaxList();
    map_.insert(OFPair<OFString, DcmTransferSyntaxList*>(skey, newentry));
    value = &newentry;
  }
  else
    value = & ((*it).second);

  // insert UID into list.
  (*value)->push_back(uid);
  return EC_Normal;
}

OFBool DcmTransferSyntaxMap::isKnownKey(const char *key) const
{
  if (!key) return OFFalse;
  if (map_.find(OFString(key)) != map_.end()) return OFTrue;
  return OFFalse;
}

const DcmTransferSyntaxList *DcmTransferSyntaxMap::getTransferSyntaxList(const char *key) const
{
  const DcmTransferSyntaxList *result = NULL;
  if (key)
  {
    OFMap<OFString, DcmTransferSyntaxList*>::const_iterator it = map_.find(OFString(key));
    if (it != map_.end())
      result = (*it).second;
  }
  return result;
}
