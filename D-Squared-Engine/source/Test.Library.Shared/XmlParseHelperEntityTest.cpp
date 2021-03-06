#include "pch.h"
#include "CppUnitTest.h"
#include "LeakDetector.h"
#include "XmlParseMaster.h"
#include "TestSharedData.h"
#include "SharedDataScope.h"
#include "Scope.h"
#include "Datum.h"
#include <glm/detail/type_vec4.hpp>
#include "XmlParseHelperEntity.h"
#include "Entity.h"
#include "World.h"
#include "Sector.h"
#include "GameTime.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Library;

namespace TestLibraryDesktop
{
    TEST_CLASS(XmlParseHelperEntityTest)
    {
    public:

        Library::EntityFactory mEntityFactory;

        TEST_METHOD_INITIALIZE(InitializeMethod)
        {
            LeakDetector::Initialize();
        }

        TEST_METHOD_CLEANUP(CleanupMethod)
        {
            LeakDetector::Finalize();
        }

        TEST_METHOD(TestParse)
        {
            Library::SharedDataScope sharedData;
            Library::XmlParseMaster parseMaster(&sharedData);
            Library::XmlParseHelperEntity helper;
            sharedData.SetXmlParseMaster(&parseMaster);
            parseMaster.AddHelper(helper);

            parseMaster.ParseFromFile("EntityGrammar.xml");

            Library::Scope* scope = sharedData.m_scope;
            World* world = scope->As<World>();
            std::string name = world != nullptr ? world->Name() : "";

            Assert::IsTrue(name == "Dales World");

            Assert::IsTrue(world->Sectors()[0].As<Sector>() != nullptr);
            Sector* sector = world->Sectors()[0].As<Sector>();
            Assert::IsTrue(sector->Name() == "Dales Sector");
            Assert::IsTrue(sector->GetParent() == world);

            Entity* entity = sector->Entities()[0].As<Entity>();
            Assert::IsTrue(entity != nullptr);
            Assert::IsTrue(entity->Name() == "Dale");

            Entity& entityRef = *entity;
            Assert::IsTrue(entityRef["Health"] == 100);
            Assert::IsTrue(entityRef["Power"] == 12.34f);
            Assert::IsTrue(entityRef["Position"] == glm::vec4(1, 2, 3, 4));
        }

        TEST_METHOD(TestWorldName)
        {
            World* world = new World();

            Assert::IsTrue(world->Name() == "");

            world->SetName("NewName");
            Assert::IsTrue(world->Name() == "NewName");

            delete world;
        }

        TEST_METHOD(TestSectorName)
        {
            Sector* sector = new Sector();

            Assert::IsTrue(sector->Name() == "");

            sector->SetName("NewName");
            Assert::IsTrue(sector->Name() == "NewName");

            delete sector;
        }

        TEST_METHOD(TestEntityName)
        {
            Entity* entity = new Entity();

            Assert::IsTrue(entity->Name() == "");

            entity->SetName("NewName");
            Assert::IsTrue(entity->Name() == "NewName");

            delete entity;
        }

        TEST_METHOD(TestCreateSector)
        {
            World* world = new World();

            Assert::IsTrue(world->Sectors().Size() == 0);

            world->CreateSector("NewSector");
            Assert::IsTrue(world->Sectors().Size() == 1);

            delete world;
        }

        TEST_METHOD(TestCreateEntity)
        {
            Sector* sector = new Sector();

            Assert::IsTrue(sector->Entities().Size() == 0);

            sector->CreateEntity("Entity", "Dale");
            Assert::IsTrue(sector->Entities().Size() == 1);

            delete sector;
        }

        TEST_METHOD(TestCloneLeaks)
        {
            // Clone for leak detection. Cloning functioality already tests in other test class
            Library::SharedDataScope sharedData;
            Library::XmlParseMaster parseMaster(&sharedData);
            Library::XmlParseHelperEntity helper;
            sharedData.SetXmlParseMaster(&parseMaster);
            parseMaster.AddHelper(helper);
            XmlParseMaster* clone = parseMaster.Clone();
            delete clone;
            Assert::IsTrue(true);
        }

        TEST_METHOD(Testupdate)
        {
            // Update has no current functionality, so test currently exists for coverage and leak detection
            World* world = new World();
            WorldState worldState;
            GameTime gameTime;
            world->Update(worldState, gameTime);

            delete world;
        }

        TEST_METHOD(TestGetWorld)
        {
            World* world = new World();

            Sector* sector = world->CreateSector("NewSector");
            Assert::IsTrue(sector->GetWorld() == *world);

            World* wrongWorld = new World();
            Assert::IsFalse(sector->GetWorld() == *wrongWorld);

            delete world;
            delete wrongWorld;
        }

        TEST_METHOD(TestGetSector)
        {
            Sector* sector = new Sector();

            Entity* entity = sector->CreateEntity("Entity", "Dale");
            Assert::IsTrue(entity->GetSector() == *sector);

            Sector* wrongSector = new Sector();
            Assert::IsFalse(entity->GetSector() == *wrongSector);

            Entity* noSectorEntity = new Entity();
            Assert::ExpectException<std::exception>([&]()
            {
                noSectorEntity->GetSector();
            });

            delete sector;
            delete wrongSector;
            delete noSectorEntity;
        }

        TEST_METHOD(TestSetWorld)
        {
            World* world1 = new World();
            Sector* sector = world1->CreateSector("mSector");

            Assert::IsTrue(sector->GetWorld() == *world1);

            World* world2 = new World();
            sector->SetWorld(*world2);
            Assert::IsTrue(sector->GetWorld() == *world2);
            Assert::IsFalse(sector->GetWorld() == *world1);

            delete world1;
            delete world2;
        }

        TEST_METHOD(TestSetSector)
        {
            Sector* sector1 = new Sector();
            Entity* entity = sector1->CreateEntity("Entity", "Dale");

            Assert::IsTrue(entity->GetSector() == *sector1);

            Sector* sector2 = new Sector();
            entity->SetSector(*sector2);
            Assert::IsTrue(entity->GetSector() == *sector2);
            Assert::IsFalse(entity->GetSector() == *sector1);

            delete sector1;
            delete sector2;
        }

    };

}