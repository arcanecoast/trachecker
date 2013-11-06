#include <fstream>

#include "HighLevelParserImpl.h"
#include "LogicError.h"

using namespace cppbg_tra;
using namespace std;

void HighLevelParserImpl::CheckStack(LowLevelParser::iterator newElement, vector<LowLevelParser::iterator>& stack)
{
    if (stack.size() == 1 && newElement->GetType() != LowLevelElement::ENTITY_TEXT) {
        throw LogicError(LogicError::ERROR_NO_TEXT_SECTION_BEFORE_SOUND_SPECIFICATION,
            newElement->GetPosition()
        );
    }

    if (stack.size() == 3) {
        if (newElement->GetType() == LowLevelElement::ENTITY_TEXT && stack[2]->GetType() == LowLevelElement::ENTITY_TEXT) {
            throw LogicError(LogicError::ERROR_THIRD_TEXT_SECTION,
                newElement->GetPosition()
            );
        }

        if (newElement->GetType() == LowLevelElement::ENTITY_SOUND && stack[2]->GetType() == LowLevelElement::ENTITY_SOUND) {
            throw LogicError(LogicError::ERROR_NO_TEXT_SECTION_BEFORE_SOUND_SPECIFICATION,
                newElement->GetPosition()
            );
        }
    }

    if (stack.size() == 4) {
        if (newElement->GetType() == LowLevelElement::ENTITY_TEXT) {
            throw LogicError(LogicError::ERROR_THIRD_TEXT_SECTION,
                newElement->GetPosition()
            );
        }

        if (stack[3]->GetType() == LowLevelElement::ENTITY_SOUND) {
            throw LogicError(LogicError::ERROR_THIRD_SOUND_SPECIFICATION,
                newElement->GetPosition()
            );
        }
    }
}

int HighLevelParserImpl::GetIntID(LowLevelParser::iterator element)
{
    char* errptr;

    int result = strtol(element->GetContent().c_str(), &errptr, 10);
    if (*errptr != 0) {
        throw LogicError(LogicError::ERROR_WRONG_INDEX_FORMAT,
            element->GetPosition()
        );
    }

    return result;
}

void HighLevelParserImpl::AggregateEntry(int EntryID, vector<LowLevelParser::iterator>& stack)
{
    switch (stack.size()) {
    case 2: //I T > I
        this->entries.insert(pair<int, shared_ptr<TranslationEntry> >(EntryID, shared_ptr<TranslationEntry>(new TranslationEntry(EntryID, stack[1]->GetContent()))));
        break;
    case 3:
        if (stack[2]->GetType() == LowLevelElement::ENTITY_SOUND) { //I T S > I
            this->entries.insert(pair<int, shared_ptr<TranslationEntry> >(EntryID, shared_ptr<TranslationEntry>(new TranslationEntry(EntryID, stack[1]->GetContent(), stack[2]->GetContent()))));
        } else { //I T T > I
            this->entries.insert(pair<int, shared_ptr<TranslationEntry> >(EntryID, shared_ptr<TranslationEntry>(new TranslationEntry(EntryID, stack[1]->GetContent(), "", stack[2]->GetContent()))));
        }
        break;
    case 4:
        if (stack[3]->GetType() == LowLevelElement::ENTITY_SOUND) { //I T T S > I
            this->entries.insert(pair<int, shared_ptr<TranslationEntry> >(EntryID, shared_ptr<TranslationEntry>(new TranslationEntry(EntryID, stack[1]->GetContent(), "", stack[2]->GetContent(), stack[3]->GetContent()))));
        } else { //I T S T > I
            this->entries.insert(pair<int, shared_ptr<TranslationEntry> >(EntryID, shared_ptr<TranslationEntry>(new TranslationEntry(EntryID, stack[1]->GetContent(), stack[2]->GetContent(), stack[3]->GetContent(), ""))));
        }
        break;
    case 5: //I T S T S > I
        this->entries.insert(pair<int, shared_ptr<TranslationEntry> >(EntryID, shared_ptr<TranslationEntry>(new TranslationEntry(EntryID, stack[1]->GetContent(), stack[2]->GetContent(), stack[3]->GetContent(), stack[4]->GetContent()))));
        break;
    }
}

void HighLevelParserImpl::LoadFromFile(const char* Filename)
{
    Clear();

    LowLevelParser baseParser;
    baseParser.LoadFromFile(Filename);

    vector<LowLevelParser::iterator> stack;
    list<LinkToEntry> assignedValues;

    for (LowLevelParser::iterator i = baseParser.Begin(); i != baseParser.End(); ++i) {
        if (i->GetType() == LowLevelElement::ENTITY_MISC) {
            continue;
        }

        if (i->GetType() == LowLevelElement::ENTITY_ID) {
            if (stack.size() > 0) {
                int idEntry = GetIntID(stack[0]);

                if (stack.size() == 1) {
                    int idAssignTo = GetIntID(i);

                    assignedValues.push_back(LinkToEntry(idEntry, idAssignTo, stack[0]));
                    stack.clear();

                    continue;
                }

                AggregateEntry(idEntry, stack);
                stack.clear();
            }
        } else if (stack.size() == 0 || stack.size() == 5) {
            throw LogicError(LogicError::ERROR_FOUND_CONTENT_NOT_ASSIGNED_TO_ENTRY,
                i->GetPosition()
            );
        }

        CheckStack(i, stack);

        stack.push_back(i);
    }

    if (stack.size() > 0) {
        if (stack.size() == 1) {
            throw LogicError(LogicError::ERROR_FOUND_CONTENT_NOT_ASSIGNED_TO_ENTRY,
                stack[0]->GetPosition()
            );
        } else {
            AggregateEntry(GetIntID(stack[0]), stack);
        }

        stack.clear();
    }

    bool assignedValuesChanged = false;
    while (!assignedValues.empty()) {
        map<int, shared_ptr<TranslationEntry> >::iterator ref;

        for (list<LinkToEntry>::iterator i = assignedValues.begin();
                i != assignedValues.end();) {
            ref = this->entries.find(i->idAssignedTo);
            if (ref != this->entries.end()) {
                this->entries.insert(pair<int, shared_ptr<TranslationEntry> >(i->id, shared_ptr<TranslationEntry>(new TranslationEntry(i->id, ref->second))));
                assignedValuesChanged = true;
                assignedValues.erase(i++);
            } else {
                ++i;
            }
        }

        if (assignedValuesChanged == false) {
            throw LogicError(LogicError::ERROR_LINK_POINTS_TO_NOWHERE,
                assignedValues.begin()->referenceToAssignee->GetPosition()
            );
        } else {
            assignedValuesChanged = false;
        }
    }
}

void HighLevelParserImpl::SaveToFile(const char* Filename) const
{
    ofstream out;
    out.open(Filename, ios_base::out | ios_base::trunc);
    if (!out.good()) {
        throw ios_base::failure("Unable to open file for writing!");
    }

    for (const_iterator i = Begin(); i != End(); ++i) {
        out << i->second->GetWritableData() << endl;
    }
}