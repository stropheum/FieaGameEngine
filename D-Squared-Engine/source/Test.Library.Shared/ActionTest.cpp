#include "pch.h"
#include "CppUnitTest.h"
#include "LeakDetector.h"
#include "TestSharedData.h"
#include "Datum.h"
#include "GameTime.h"
#include "Factory.h"
#include "World.h"
#include "Sector.h"
#include "Entity.h"
#include "Action.h"
#include "ActionList.h"
#include "WorldState.h"
#include "ActionCreateAction.h"
#include "Scope.h"
#include "ActionDestroyAction.h"
#include "ActionListIf.h"
#include "SharedDataScope.h"
#include "XmlParseHelperEntity.h"
#include "ActionIncrement.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Library;
using namespace std;


namespace TestLibraryDesktop
{
    TEST_CLASS(ActionTest)
    {
    public:

        EntityFactory mEntityFactory;
        ActionListFactory mActionListFactory;
        ActionCreateActionFactory mActionCreateActionFactory;
        ActionDestroyActionFactory mActionDestroyActionFactory;
        ActionListIfFactory mActionListIfFactory;
        ActionIncrementFactory mActionIncrementFactory;

        TEST_METHOD_INITIALIZE(InitializeMethod)
        {
            LeakDetector::Initialize();
        }

        TEST_METHOD_CLEANUP(CleanupMethod)
        {
            LeakDetector::Finalize();
        }

        TEST_METHOD(UpdateLeakTest)
        {
            World* world = new World();
            Sector* sector = world->CreateSector("Dales Sector");
            Entity* entity = sector->CreateEntity("Entity", "Dale");
            Action* action = entity->CreateAction("ActionList", "DoThing");

            WorldState worldState;
            GameTime gameTime;

            world->Update(worldState, gameTime);
            Assert::IsTrue(world != nullptr && sector != nullptr && entity != nullptr && action != nullptr);

            delete world;
        }

        TEST_METHOD(TestCreateAction)
        {
            Entity* entity = new Entity();
            Action* action = entity->CreateAction("ActionList", "newAction");
            Assert::IsTrue(action != nullptr);
            delete entity;
        }

        TEST_METHOD(TestActionIncrement)
        {
            WorldState worldState;
            Entity* entity = new Entity();
            Action* action = entity->CreateAction("ActionIncrement", "newAction");
            Assert::IsTrue(action != nullptr);

            ActionIncrement* actionIncrement = dynamic_cast<ActionIncrement*>(action);
            Assert::IsTrue(actionIncrement != nullptr);
            Assert::AreEqual(0, actionIncrement->GetIncrementCount());

            actionIncrement->Update(worldState);
            Assert::AreEqual(1, actionIncrement->GetIncrementCount());

            actionIncrement->Update(worldState);
            Assert::AreEqual(2, actionIncrement->GetIncrementCount());

            delete entity;
        }

        TEST_METHOD(TestName)
        {
            ActionList actionList;
            Assert::IsTrue(actionList.Name() == "");
            actionList.SetName("Dale");
            Assert::IsTrue(actionList.Name() == "Dale");
        }

        TEST_METHOD(TestActionCreateAction)
        {
            WorldState worldState;

            ActionList aList;
            aList.CreateAction("ActionCreateAction", "ActionCreateActionGenerically");

            ActionCreateAction* aca1 = aList["Actions"].Get<Scope>(0).As<ActionCreateAction>();
            Assert::IsTrue(aca1 != nullptr);
            aca1->SetPrototype("ActionList");
            aca1->SetName("MyActionList");
            aList.Update(worldState);

            Assert::IsTrue(aList["Actions"].Get<Scope>(1).Is(ActionList::TypeIdClass()));
        }

        TEST_METHOD(TestActionDestroyAction)
        {
            WorldState worldState;

            ActionList actionList;
            actionList.CreateAction("ActionList", "MyActionList");

            ActionList* childActionList = actionList["Actions"].Get<Scope>(0).As<ActionList>();
            Assert::IsTrue(childActionList != nullptr);

            ActionDestroyAction* destroyAction =
                childActionList->CreateAction("ActionDestroyAction", "MyDestroyAction")->As<ActionDestroyAction>();
            destroyAction->SetActionInstanceName("MyActionList");

            actionList.Update(worldState);
        }

        TEST_METHOD(TestActionListIf)
        {
            WorldState worldState;

            ActionList actionList;
            ActionListIf* ifList = actionList.CreateAction("ActionListIf", "MyIf")->As<ActionListIf>();

            ifList->SetCondition(true);
            actionList.Update(worldState);
            ifList->SetCondition(false);
            actionList.Update(worldState);
        }

        TEST_METHOD(TestParse)
        {
            WorldState state;
            GameTime time;
            Library::SharedDataScope sharedData;
            Library::XmlParseMaster parseMaster(&sharedData);
            Library::XmlParseHelperEntity helper;
            sharedData.SetXmlParseMaster(&parseMaster);
            parseMaster.AddHelper(helper);

            parseMaster.ParseFromFile("ActionGrammar.xml");

            World* world = sharedData.m_scope->As<World>();
            Assert::IsTrue(world != nullptr);
            Assert::IsTrue(world->Name() == "Dales World");

            Sector* sector = world->Sectors().Get<Scope>(0).As<Sector>();
            Assert::IsTrue(sector != nullptr);
            Assert::IsTrue(sector->Name() == "Dales Sector");

            Entity* entity = sector->Entities().Get<Scope>(0).As<Entity>();
            Assert::IsTrue(entity != nullptr);
            Assert::IsTrue(entity->Name() == "Dale");

            Action* action = entity->Actions().Get<Scope>(0).As<Action>();
            Assert::IsTrue(action != nullptr);
            Assert::IsTrue(action->Name() == "MyList");
            Assert::IsTrue(action->Is(ActionListIf::TypeIdClass()));

            Action* then = (*action->As<ActionList>())["Then"].Get<Scope>(0).As<Action>();
            Assert::IsTrue(then != nullptr);

            Assert::IsTrue(then->Is(ActionIncrement::TypeIdClass()));

            Assert::AreEqual(then->As<ActionIncrement>()->GetIncrementCount(), 0);
            world->Update(state, time);

            Assert::AreEqual(then->As<ActionIncrement>()->GetIncrementCount(), 1);
        }

    };

}