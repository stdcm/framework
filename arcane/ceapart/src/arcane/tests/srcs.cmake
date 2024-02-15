set(ARCANE_SOURCES
  RayMeshIntersectionUnitTest.cc
  CartesianMeshTesterModule.cc
  GeometricUnitTest.cc
  AMRCartesianMeshTesterModule.cc
  CartesianMeshTestUtils.cc
  CartesianMeshTestUtils.h
  CartesianMeshV2TestUtils.cc
  CartesianMeshV2TestUtils.h
  UnitTestCartesianMeshPatch.cc
  IMaterialEquationOfState.h
)

set(ARCANE_MATERIAL_SOURCES
  HyodaMixedCellsUnitTest.cc
  MeshMaterialTesterModule.cc
  MeshMaterialSyncUnitTest.cc
  MeshMaterialSimdUnitTest.cc
  MaterialHeatTestModule.cc
)

set(AXL_FILES 
  RayMeshIntersectionUnitTest
  MaterialHeatTest
  MeshMaterialTester
  CartesianMeshTester
  AdiProjection
  HyodaMixedCellsUnitTest
  GeometricUnitTest
  MeshMaterialSyncUnitTest
  AMRCartesianMeshTester
  UnitTestCartesianMeshPatch
)
