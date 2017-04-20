#include "pch.h"
#include "ReactionAttributed.h"
#include "Event.h"
#include "EventMessageAttributed.h"
#include "World.h"
#include "Scope.h"


namespace Library
{
	ReactionAttributed::ReactionAttributed()
	{
		(*this)["Subtype"].setType(DatumType::String);
		(*this)["Subtype"] = "";
		mPrescribedAttributes.pushBack(
			Signature("Subtype", DatumType::String, 1u, static_cast<std::string*>(nullptr)));
		Event<EventMessageAttributed>::subscribe(*this);
	}

	ReactionAttributed::~ReactionAttributed()
	{
		Event<EventMessageAttributed>::unsubscribe(*this);
	}

	void ReactionAttributed::notify(const EventPublisher& event)
	{
		assert(event.Is(Event<EventMessageAttributed>::TypeIdClass()));

		auto castEvent = event.As<Event<EventMessageAttributed>>();
		
		assert(castEvent->message().Is(EventMessageAttributed::TypeIdClass()));
		EventMessageAttributed* message = castEvent->message().As<EventMessageAttributed>();

		auto lhs = message->getSubtype();
		auto rhs = getSubtype();
//		if (message->getSubtype() == getSubtype())
		if (lhs == rhs)
		{
			for (auto iter = message->begin(); iter != message->end(); ++iter)
			{
				auto& datum = appendAuxiliaryAttribute((*iter).first);
				datum.setType((*iter).second.type());
				datum = (*iter).second;
			}
		}
	}

	std::string ReactionAttributed::getSubtype()
	{
		return (*this)["Subtype"].get<std::string>(0);
	}

	void ReactionAttributed::setSubtype(const std::string& subtype)
	{
		(*this)["Subtype"] = subtype;
	}
}
