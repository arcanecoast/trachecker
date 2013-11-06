#include "HighLevelParserImpl.h"
#include "HighLevelParser.h"

using namespace cppbg_tra;

HighLevelParser::HighLevelParser() : pimpl(new HighLevelParserImpl()) {}

void HighLevelParser::LoadFromFile(const char* Filename) { pimpl->LoadFromFile(Filename); }
void HighLevelParser::SaveToFile(const char* Filename) const { pimpl->SaveToFile(Filename); }

size_t HighLevelParser::Size() const { return pimpl->Size(); }

HighLevelParser::const_iterator HighLevelParser::GetEntry(int ID) const { return pimpl->GetEntry(ID); }
HighLevelParser::iterator HighLevelParser::GetEntry(int ID) { return pimpl->GetEntry(ID); }

HighLevelParser::iterator HighLevelParser::Begin() { return pimpl->Begin(); }
HighLevelParser::iterator HighLevelParser::End() { return pimpl->End(); }

HighLevelParser::const_iterator HighLevelParser::Begin() const { return pimpl->Begin(); }
HighLevelParser::const_iterator HighLevelParser::End() const { return pimpl->End(); }

void HighLevelParser::Clear() { pimpl->Clear(); }