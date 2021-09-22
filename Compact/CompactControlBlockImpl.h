#pragma once
#include "ICompactControlBlock.h"
#include "CompactImpl.h"

class CompactImpl;

class CompactControlBlockImpl : public ICompactControlBlock 
{
  public:
	CompactControlBlockImpl();
	CompactControlBlockImpl(CompactImpl* const& ent) : entity(ent) {};
    RC get(IMultiIndex *const &currentIndex, IMultiIndex const *const &bypassOrder) const override;
    RC get(IMultiIndex const *const &currentIndex, IVector *const &val) const override;
  private:
    CompactImpl* const entity;
};
