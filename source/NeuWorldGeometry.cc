#include "NeuWorldGeometry.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"

#include "NeuMaterials.hh"


#include "G4SDManager.hh"


NeuFlux::NeuWorldGeometry::NeuWorldGeometry() : G4VUserDetectorConstruction(), fDetector(NULL), 
												fWorldX(1.*m),fWorldY(1.*m),fWorldZ(1.*m),
												fRockX(1.*m),fRockY(0.9*m),fRockZ(1.*m),
												fConcreteX(0.15*m),fConcreteY(0.15*m),fConcreteZ(0.15*m),
												fDetectorX(0.1*m),fDetectorY(0.1*m),fDetectorZ(0.1*m)
{
	fMessenger = new NeuFlux::NeuGeometryMessenger(this);	
}

NeuFlux::NeuWorldGeometry::~NeuWorldGeometry()
{
	delete fMessenger;
}

G4VPhysicalVolume* NeuFlux::NeuWorldGeometry::Construct()
{
	if (fPhysiWorld) 
	{
	  G4GeometryManager::GetInstance()->OpenGeometry();
	  G4PhysicalVolumeStore::GetInstance()->Clean();
	  G4LogicalVolumeStore::GetInstance()->Clean();
	  G4SolidStore::GetInstance()->Clean();
	}
	return ConstructWorld();
}

/*
G4int FindVertexVolumeIndex(const G4LogicalVolume * vertexLogicalVolume)
{
	return 0;
}
G4bool NeuFlux::NeuWorldGeometry::StoreEnteringParticleInfo(G4VPhysicalVolume * postVolume)
{
	return false;
}
G4int NeuFlux::NeuWorldGeometry::FindPhysicalVolumeIndex(G4VPhysicalVolume * whichVolume)
{
	return 0;
}
*/

G4VPhysicalVolume* NeuFlux::NeuWorldGeometry::ConstructWorld()
{
	G4NistManager* man = G4NistManager::Instance();
	fLogicWorld = new G4LogicalVolume(
						new G4Box("World",
	                    fWorldX*0.5,
	                    fWorldY*0.5, fWorldZ*0.5),
	                man->FindOrBuildMaterial("G4_AIR"),
	                "World");

	fPhysiWorld = new G4PVPlacement(0,
					G4ThreeVector(),
					fLogicWorld, 
					"World", 
					0, false, 0);
	ConstructRock();
	return fPhysiWorld;
}
G4VPhysicalVolume* NeuFlux::NeuWorldGeometry::ConstructRock()
{
	fLogicRock = new G4LogicalVolume(
						new G4Box("Rock",
	                    fRockX*0.5,
	                    fRockY*0.5, fRockZ*0.5),
	                new NeuRock(),
	                "Rock");

	fPhysiRock = new G4PVPlacement(0,
					G4ThreeVector(   0.0,(fRockY-fWorldY)*2.0, 0.0 ),
					fLogicRock, 
					"Rock", 
					fLogicWorld, 
					false, 
					0);
	ConstructConcrete();
	return fPhysiRock;
}
G4VPhysicalVolume* NeuFlux::NeuWorldGeometry::ConstructConcrete()
{
	fLogicConcrete = new G4LogicalVolume(
						new G4Box("Concrete",
	                    fConcreteX*0.5,
	                    fConcreteY*0.5, fConcreteZ*0.5),
	                new NeuConcrete(),
	                "Concrete");

	fPhysiConcrete = new G4PVPlacement(0,
					G4ThreeVector(  0.0,(fConcreteY-fRockY)*2.0,  0.0 ),
					fLogicConcrete, 
					"Concrete", 
					fLogicRock, 
					false, 
					0);
	ConstructDetector();
	return fPhysiConcrete;
}
G4VPhysicalVolume* NeuFlux::NeuWorldGeometry::ConstructDetector()
{
	G4NistManager* man = G4NistManager::Instance();

	fLogicDetector = new G4LogicalVolume(
						new G4Box("Detector",
	                    fDetectorX*0.5,
	                    fDetectorY*0.5, fDetectorZ*0.5),
	                man->FindOrBuildMaterial("G4_POLYTRIFLUOROCHLOROETHYLENE"),
	                "Detector");

	fPhysiDetector = new G4PVPlacement(0,
					G4ThreeVector( 0.0,(fDetectorY-fConcreteY)*2.0 ,   0.0 ),
					fLogicDetector, 
					"Detector", 
					fLogicConcrete, 
					false, 
					0);



	G4SDManager *SDman = G4SDManager::GetSDMpointer();
	if(!fDetector)
	{
		fDetector = new NeuFlux::NeuDetector();	
		SDman->AddNewDetector(fDetector);	
		fLogicDetector->SetSensitiveDetector(fDetector);
	}
	return fPhysiDetector;
}

void NeuFlux::NeuWorldGeometry::PrintGeometry()
{
	G4cout<<"World Geometry: "
		 <<fWorldX<<" , "		
		 <<fWorldY<<" , "			
		 <<fWorldZ<<std::endl;			

	G4cout<<"Rock Geometry: "
		 <<fRockX<<" , "			
		 <<fRockY<<" , "			
		 <<fRockZ<<std::endl;		

	G4cout<<"Concrete Geometry: "
		 <<fConcreteX<<" , "		
		 <<fConcreteY<<" , "		
		 <<fConcreteZ<<std::endl;	
	G4cout<<"Detector Geometry: "
		 <<fDetectorX<<" , "		
		 <<fDetectorY<<" , "		
		 <<fDetectorZ<<std::endl;		
}

