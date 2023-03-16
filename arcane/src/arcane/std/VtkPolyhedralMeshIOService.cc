// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2023 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* VtkPolyhedralMeshIOService                      (C) 2000-2023             */
/*                                                                           */
/* Read/write fools for polyhedral mesh with vtk file format                 */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#include <iostream>
#include <numeric>
#include <functional>

#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkNew.h>
#include <vtkCellIterator.h>
#include <vtkIdTypeArray.h>

#include <arccore/base/Ref.h>
#include <arccore/base/String.h>
#include <arccore/base/FatalErrorException.h>

#include "arcane/core/ArcaneTypes.h"
#include "arcane/core/AbstractService.h"
#include "arcane/core/ICaseMeshReader.h"
#include "arcane/core/ServiceFactory.h"
#include "arcane/core/IMeshBuilder.h"
#include "arcane/core/MeshBuildInfo.h"
#include "arcane/core/IPrimaryMesh.h"
#include "arcane/core/MeshUtils.h"
#include "arcane/core/IMeshInitialAllocator.h"
#include "arcane/utils/ArcaneGlobal.h"
#include "arcane/utils/ITraceMng.h"
#include "arcane/utils/UniqueArray.h"
#include "arcane/utils/Real3.h"

#include "arcane/core/ItemAllocationInfo.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

namespace VtkPolyhedralTools
{
  struct ReadStatus
  {
    bool failure = false;
    String failure_message;
  };
} // namespace VtkPolyhedralTools

class VtkPolyhedralMeshIOService
{

  class VtkReader
  {

   public:

    explicit VtkReader(const String& filename);

    Int64ConstArrayView cellUids();
    Int64ConstArrayView nodeUids();
    Int64ConstArrayView faceUids();
    Int64ConstArrayView edgeUids();

    Integer nbNodes();

    Int64ConstArrayView cellNodes();
    Int32ConstArrayView cellNbNodes();

    Int64ConstArrayView faceNodes();
    Int32ConstArrayView faceNbNodes();

    Int64ConstArrayView edgeNodes();
    Int32ConstArrayView edgeNbNodes();

    Int64ConstArrayView faceCells();
    Int32ConstArrayView faceNbCells();

    Int32ConstArrayView edgeNbCells();
    Int64ConstArrayView edgeCells();

    Int32ConstArrayView cellNbFaces();
    Int64ConstArrayView cellFaces();

    Int32ConstArrayView edgeNbFaces();
    Int64ConstArrayView edgeFaces();

    Int32ConstArrayView cellNbEdges();
    Int64ConstArrayView cellEdges();

    Int32ConstArrayView faceNbEdges();
    Int64ConstArrayView faceEdges();

    Int32ConstArrayView nodeNbCells();
    Int64ConstArrayView nodeCells();

    Int32ConstArrayView nodeNbFaces();
    Int64ConstArrayView nodeFaces();

    Int32ConstArrayView nodeNbEdges();
    Int64ConstArrayView nodeEdges();

    Real3ArrayView nodeCoords();

    bool readHasFailed() const noexcept { return m_read_status.failure; }
    const VtkPolyhedralTools::ReadStatus readStatus() const noexcept { return m_read_status; }

   private:

    const String& m_filename;
    VtkPolyhedralTools::ReadStatus m_read_status;
    vtkNew<vtkUnstructuredGridReader> m_vtk_grid_reader;
    Int64UniqueArray m_cell_uids, m_node_uids, m_face_uids, m_edge_uids;
    Int64UniqueArray m_face_node_uids, m_edge_node_uids, m_cell_node_uids;
    Int64UniqueArray m_face_cell_uids, m_edge_cell_uids, m_edge_face_uids;
    Int64UniqueArray m_cell_face_uids, m_cell_edge_uids, m_face_edge_uids;
    Int64UniqueArray m_node_cell_uids, m_node_face_uids, m_node_edge_uids;
    Int32UniqueArray m_face_nb_nodes, m_cell_nb_nodes, m_face_nb_cells;
    Int32UniqueArray m_edge_nb_cells, m_edge_nb_faces, m_cell_nb_faces;
    Int32UniqueArray m_node_nb_cells, m_node_nb_faces, m_node_nb_edges;
    Int32UniqueArray m_cell_nb_edges, m_face_nb_edges, m_face_uid_indexes;
    Int32UniqueArray m_cell_face_indexes, m_edge_nb_nodes;
    Real3UniqueArray m_node_coordinates;

    std::pair<bool, Int32> _findFace(Int64ConstArrayView face_nodes, Int64ConstArrayView face_node_uids, Int32ConstArrayView face_nb_nodes);
    template <typename Connectivity2DArray>
    void _flattenConnectivity(Connectivity2DArray connected_item_2darray, Int32Span nb_connected_item_per_source_item, Int64UniqueArray& connected_item_array);
  };

 public:

  void _fillItemAllocationInfo(ItemAllocationInfo& item_allocation_info, VtkReader& vtk_reader)
  {
    auto nb_item_family = 4;
    auto nb_connected_family = 3;
    item_allocation_info.family_infos.resize(nb_item_family);
    for (auto& family_info : item_allocation_info.family_infos) {
      family_info.connected_family_info.resize(nb_connected_family);
    }
    // Create regular item families and connectivities
    auto& cell_family_info = item_allocation_info.family_infos[0];
    cell_family_info.name = "Cell";
    cell_family_info.item_kind = IK_Cell;
    cell_family_info.item_uids = vtk_reader.cellUids();
    auto& node_family_info = item_allocation_info.family_infos[1];
    node_family_info.name = "Node";
    node_family_info.item_kind = IK_Node;
    node_family_info.item_uids = vtk_reader.nodeUids();
    auto& face_family_info = item_allocation_info.family_infos[2];
    face_family_info.name = "Face";
    face_family_info.item_kind = IK_Face;
    face_family_info.item_uids = vtk_reader.faceUids();
    auto& edge_family_info = item_allocation_info.family_infos[3];
    edge_family_info.name = "Edge";
    edge_family_info.item_kind = IK_Edge;
    edge_family_info.item_uids = vtk_reader.edgeUids();
    // Cell to nodes connectivity
    auto cell_connected_family_index = 0;
    auto& cell_connected_node_family_info = cell_family_info.connected_family_info[cell_connected_family_index++];
    cell_connected_node_family_info.name = node_family_info.name;
    cell_connected_node_family_info.item_kind = node_family_info.item_kind;
    cell_connected_node_family_info.connectivity_name = "CellToNodes";
    cell_connected_node_family_info.nb_connected_items_per_item = vtk_reader.cellNbNodes();
    cell_connected_node_family_info.connected_items_uids = vtk_reader.cellNodes();
    // Cell to faces connectivity
    auto& cell_connected_face_family_info = cell_family_info.connected_family_info[cell_connected_family_index++];
    cell_connected_face_family_info.name = face_family_info.name;
    cell_connected_face_family_info.item_kind = face_family_info.item_kind;
    cell_connected_face_family_info.connectivity_name = "CellToFaces";
    cell_connected_face_family_info.nb_connected_items_per_item = vtk_reader.cellNbFaces();
    cell_connected_face_family_info.connected_items_uids = vtk_reader.cellFaces();
    // Cell to edges connectivity
    auto& cell_connected_edge_family_info = cell_family_info.connected_family_info[cell_connected_family_index++];
    cell_connected_edge_family_info.name = edge_family_info.name;
    cell_connected_edge_family_info.item_kind = edge_family_info.item_kind;
    cell_connected_edge_family_info.connectivity_name = "CellToEdges";
    cell_connected_edge_family_info.nb_connected_items_per_item = vtk_reader.cellNbEdges();
    cell_connected_edge_family_info.connected_items_uids = vtk_reader.cellEdges();
    // Face to cells connectivity
    auto face_connected_family_index = 0;
    auto& face_connected_cell_family_info = face_family_info.connected_family_info[face_connected_family_index++];
    face_connected_cell_family_info.name = cell_family_info.name;
    face_connected_cell_family_info.item_kind = cell_family_info.item_kind;
    face_connected_cell_family_info.connectivity_name = "FaceToCells";
    face_connected_cell_family_info.nb_connected_items_per_item = vtk_reader.faceNbCells();
    face_connected_cell_family_info.connected_items_uids = vtk_reader.faceCells();
    // Face to nodes connectivity
    auto& face_connected_node_family_info = face_family_info.connected_family_info[face_connected_family_index++];
    face_connected_node_family_info.name = node_family_info.name;
    face_connected_node_family_info.item_kind = node_family_info.item_kind;
    face_connected_node_family_info.connectivity_name = "FaceToNodes";
    face_connected_node_family_info.nb_connected_items_per_item = vtk_reader.faceNbNodes();
    face_connected_node_family_info.connected_items_uids = vtk_reader.faceNodes();
    // Face to edges connectivity
    auto& face_connected_edge_family_info = face_family_info.connected_family_info[face_connected_family_index];
    face_connected_edge_family_info.name = edge_family_info.name;
    face_connected_edge_family_info.item_kind = edge_family_info.item_kind;
    face_connected_edge_family_info.connectivity_name = "FaceToEdges";
    face_connected_edge_family_info.nb_connected_items_per_item = vtk_reader.faceNbEdges();
    face_connected_edge_family_info.connected_items_uids = vtk_reader.faceEdges();
    // Edge to cells connectivity
    auto edge_connected_family_index = 0;
    auto& edge_connected_cell_family_info = edge_family_info.connected_family_info[edge_connected_family_index++];
    edge_connected_cell_family_info.name = cell_family_info.name;
    edge_connected_cell_family_info.item_kind = cell_family_info.item_kind;
    edge_connected_cell_family_info.connectivity_name = "EdgeToCells";
    edge_connected_cell_family_info.nb_connected_items_per_item = vtk_reader.edgeNbCells();
    edge_connected_cell_family_info.connected_items_uids = vtk_reader.edgeCells();
    // Edge to faces connectivity
    auto& edge_connected_face_family_info = edge_family_info.connected_family_info[edge_connected_family_index++];
    edge_connected_face_family_info.name = face_family_info.name;
    edge_connected_face_family_info.item_kind = face_family_info.item_kind;
    edge_connected_face_family_info.connectivity_name = "EdgeToFaces";
    edge_connected_face_family_info.nb_connected_items_per_item = vtk_reader.edgeNbFaces();
    edge_connected_face_family_info.connected_items_uids = vtk_reader.edgeFaces();
    // Edge to nodes connectivity
    auto& edge_connected_node_family_info = edge_family_info.connected_family_info[edge_connected_family_index++];
    edge_connected_node_family_info.name = node_family_info.name;
    edge_connected_node_family_info.item_kind = node_family_info.item_kind;
    edge_connected_node_family_info.connectivity_name = "EdgeToNodes";
    edge_connected_node_family_info.nb_connected_items_per_item = vtk_reader.edgeNbNodes();
    edge_connected_node_family_info.connected_items_uids = vtk_reader.edgeNodes();
    // Node to cells connectivity
    auto node_connected_family_index = 0;
    auto& node_connected_cell_family_info = node_family_info.connected_family_info[node_connected_family_index++];
    node_connected_cell_family_info.name = cell_family_info.name;
    node_connected_cell_family_info.item_kind = cell_family_info.item_kind;
    node_connected_cell_family_info.connectivity_name = "NodeToCells";
    node_connected_cell_family_info.nb_connected_items_per_item = vtk_reader.nodeNbCells();
    node_connected_cell_family_info.connected_items_uids = vtk_reader.nodeCells();
    // Node to faces connectivity
    auto& node_connected_face_family_info = node_family_info.connected_family_info[node_connected_family_index++];
    node_connected_face_family_info.name = face_family_info.name;
    node_connected_face_family_info.item_kind = face_family_info.item_kind;
    node_connected_face_family_info.connectivity_name = "NodeToFaces";
    node_connected_face_family_info.nb_connected_items_per_item = vtk_reader.nodeNbFaces();
    node_connected_face_family_info.connected_items_uids = vtk_reader.nodeFaces();
    // Node to edges connectivity
    auto& node_connected_edge_family_info = node_family_info.connected_family_info[node_connected_family_index++];
    node_connected_edge_family_info.name = edge_family_info.name;
    node_connected_edge_family_info.item_kind = edge_family_info.item_kind;
    node_connected_edge_family_info.connectivity_name = "NodeToEdges";
    node_connected_edge_family_info.nb_connected_items_per_item = vtk_reader.nodeNbEdges();
    node_connected_edge_family_info.connected_items_uids = vtk_reader.nodeEdges();
    // Node coordinates
    node_family_info.item_coordinates_variable_name = "NodeCoord";
    node_family_info.item_coordinates = vtk_reader.nodeCoords();
  }

  VtkPolyhedralTools::ReadStatus read(IPrimaryMesh* mesh, const String& filename)
  {
    ARCANE_CHECK_POINTER(mesh);
    VtkReader reader{ filename };
    if (reader.readHasFailed())
      return reader.readStatus();
    ItemAllocationInfo item_allocation_info;
    _fillItemAllocationInfo(item_allocation_info, reader);
    auto polyhedral_mesh_allocator = mesh->initialAllocator()->polyhedralMeshAllocator();
    polyhedral_mesh_allocator->allocateItems(item_allocation_info);
    return reader.readStatus();
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class VtkPolyhedralCaseMeshReader
: public AbstractService
, public ICaseMeshReader
{
 public:

  class Builder : public IMeshBuilder
  {
   public:

    explicit Builder(ITraceMng* tm, const CaseMeshReaderReadInfo& read_info)
    : m_trace_mng(tm)
    , m_read_info(read_info)
    {}

   public:

    void fillMeshBuildInfo(MeshBuildInfo& build_info) override
    {
      build_info.addFactoryName("ArcanePolyhedralMeshFactory");
      build_info.addNeedPartitioning(false);
    }

    void allocateMeshItems(IPrimaryMesh* pm) override
    {
      ARCANE_CHECK_POINTER(pm);
      m_trace_mng->info() << "---CREATE POLYHEDRAL MESH---- " << pm->name();
      m_trace_mng->info() << "--Read mesh file " << m_read_info.fileName();
      VtkPolyhedralMeshIOService polyhedral_vtk_service{};
      auto read_status = polyhedral_vtk_service.read(pm, m_read_info.fileName());
      if (read_status.failure)
        ARCANE_FATAL(read_status.failure_message);
    }

   private:

    ITraceMng* m_trace_mng;
    CaseMeshReaderReadInfo m_read_info;
  };

  explicit VtkPolyhedralCaseMeshReader(const ServiceBuildInfo& sbi)
  : AbstractService(sbi)
  {}

 public:

  Ref<IMeshBuilder> createBuilder(const CaseMeshReaderReadInfo& read_info) const override
  {
    IMeshBuilder* builder = nullptr;
    if (read_info.format() == "vtk")
      builder = new Builder(traceMng(), read_info);
    return makeRef(builder);
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE(VtkPolyhedralCaseMeshReader,
                        ServiceProperty("VtkPolyhedralCaseMeshReader", ST_CaseOption),
                        ARCANE_SERVICE_INTERFACE(ICaseMeshReader));

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

VtkPolyhedralMeshIOService::VtkReader::
VtkReader(const String& filename)
: m_filename{ filename }
{
  if (filename.empty()) {
    m_read_status.failure = true;
    m_read_status.failure_message = "filename for polyhedral vtk mesh is empty.";
    return;
  }
  m_vtk_grid_reader->SetFileName(filename.localstr());
  m_vtk_grid_reader->Update();
  auto* vtk_grid = m_vtk_grid_reader->GetOutput();
  if (!vtk_grid) {
    m_read_status.failure = true;
    m_read_status.failure_message = String::format("Cannot read vtk polyhedral file {0}", filename);
    return;
  }

  std::cout << "-- VTK GRID READ "
            << " NB CELLS  " << vtk_grid->GetNumberOfCells() << std::endl;
  // Parse cells
  auto* cell_iter = vtk_grid->vtkDataSet::NewCellIterator();
  cell_iter->InitTraversal();
  vtkIdType* cell_faces{ nullptr };
  vtkIdType nb_faces = 0;
  while (!cell_iter->IsDoneWithTraversal()) {
    std::cout << "---- visiting cell id " << cell_iter->GetCellId() << std::endl;
    std::cout << "----   cell number of faces " << cell_iter->GetNumberOfFaces() << std::endl;
    std::cout << "----   cell number of points " << cell_iter->GetNumberOfPoints() << std::endl;
    vtk_grid->GetFaceStream(cell_iter->GetCellId(), nb_faces, cell_faces);
    for (auto iface = 0; iface < nb_faces; ++iface) {
      auto face_nb_nodes = *cell_faces++;
      std::cout << "----      has face with " << face_nb_nodes << " nodes. Node ids : ";
      for (int inode = 0; inode < face_nb_nodes; ++inode) {
        std::cout << *cell_faces++;
      }
      std::cout << std::endl;
    }
    cell_iter->GoToNextCell();
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
cellUids()
{
  if (m_cell_uids.empty()) {
    auto* vtk_grid = m_vtk_grid_reader->GetOutput();
    m_cell_uids.reserve(vtk_grid->GetNumberOfCells());
    m_cell_nb_nodes.reserve(vtk_grid->GetNumberOfCells());
    m_cell_node_uids.reserve(10 * vtk_grid->GetNumberOfCells()); // take a mean of 10 nodes per cell
    auto* cell_iter = vtk_grid->NewCellIterator();
    cell_iter->InitTraversal();
    while (!cell_iter->IsDoneWithTraversal()) {
      m_cell_uids.push_back(cell_iter->GetCellId());
      m_cell_nb_nodes.push_back(Integer(cell_iter->GetNumberOfPoints()));
      ArrayView<vtkIdType> cell_nodes{ Integer(cell_iter->GetNumberOfPoints()), cell_iter->GetPointIds()->GetPointer(0) };
      std::for_each(cell_nodes.begin(), cell_nodes.end(), [this](auto uid) { this->m_cell_node_uids.push_back(uid); });
      cell_iter->GoToNextCell();
    }
  }
  return m_cell_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
nodeUids()
{
  if (m_node_uids.empty()) {
    auto* vtk_grid = m_vtk_grid_reader->GetOutput();
    auto nb_nodes = vtk_grid->GetNumberOfPoints();
    m_node_uids.reserve(nb_nodes);
    m_node_nb_cells.reserve(nb_nodes);
    m_node_cell_uids.reserve(8 * nb_nodes);
    for (int node_index = 0; node_index < nb_nodes; ++node_index) {
      m_node_uids.push_back(node_index);
      auto cell_nodes = vtkIdList::New();
      vtk_grid->GetPointCells(node_index, cell_nodes);
      Int64Span cell_nodes_view((Int64*)cell_nodes->GetPointer(0), cell_nodes->GetNumberOfIds());
      m_node_cell_uids.addRange(cell_nodes_view);
      m_node_nb_cells.push_back((Int32)cell_nodes->GetNumberOfIds());
    }
  }
  return m_node_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
faceUids()
{
  if (m_face_uids.empty()) {
    auto* vtk_grid = m_vtk_grid_reader->GetOutput();
    auto* cell_iter = vtk_grid->NewCellIterator();
    cell_iter->InitTraversal();
    vtkIdType nb_face_estimation = 0;
    while (!cell_iter->IsDoneWithTraversal()) {
      vtkIdType cell_nb_faces = 0;
      vtkIdType* points{ nullptr };
      vtk_grid->GetFaceStream(cell_iter->GetCellId(), cell_nb_faces, points);
      nb_face_estimation += cell_nb_faces;
      cell_iter->GoToNextCell();
    }
    m_face_uids.reserve(nb_face_estimation);
    auto* faces = vtk_grid->GetFaces();
    // This array contains the face info per cells (cf. vtk file)
    // first_cell_nb_faces first_cell_first_face_nb_nodes first_cell_first_face_node_1 ... first_cell_first_face_node_n first_cell_second_face_nb_nodes etc

    if (!faces) {
      ARCANE_FATAL("Mesh {0} is not polyhedral: faces are not defined", m_filename);
    }
    Int64 face_uid = 0;
    auto face_info_size = faces->GetNumberOfValues();
    m_face_node_uids.reserve(face_info_size);
    m_face_nb_nodes.reserve(nb_face_estimation);
    m_face_cell_uids.reserve(2 * nb_face_estimation);
    m_face_nb_cells.reserve(nb_face_estimation);
    m_cell_face_uids.reserve(8 * m_cell_uids.size()); // take a mean of 8 faces per cell
    m_cell_nb_faces.resize(m_cell_uids.size(), 0);
    m_cell_face_indexes.resize(m_cell_uids.size(), -1);
    m_face_uid_indexes.resize(2 * nb_face_estimation, -1);
    Int64UniqueArray current_face_nodes, sorted_current_face_nodes;
    current_face_nodes.reserve(10);
    sorted_current_face_nodes.reserve(10);
    UniqueArray<std::set<Int64>> node_faces(m_node_uids.size());
    auto cell_index = 0;
    auto cell_face_index = 0;
    auto global_face_index = 0;
    auto face_uid_index = 0;
    for (int face_info_index = 0; face_info_index < face_info_size; cell_index++) { // face data are given by cell
      auto current_cell_nb_faces = Int32(faces->GetValue(face_info_index++));
      m_cell_face_indexes[m_cell_uids[cell_index]] = cell_face_index;
      for (auto face_index = 0; face_index < current_cell_nb_faces; ++face_index, ++global_face_index) {
        auto current_face_nb_nodes = Int32(faces->GetValue(face_info_index++));
        m_cell_nb_faces[m_cell_uids[cell_index]] += 1;
        for (int node_index = 0; node_index < current_face_nb_nodes; ++node_index) {
          current_face_nodes.push_back(faces->GetValue(face_info_index++));
        }
        sorted_current_face_nodes.resize(current_face_nodes.size());
        auto is_front_cell = mesh_utils::reorderNodesOfFace(current_face_nodes, sorted_current_face_nodes);
        auto [is_face_found, existing_face_index] = _findFace(sorted_current_face_nodes, m_face_node_uids, m_face_nb_nodes);
        if (!is_face_found) {
          for (auto node : current_face_nodes) {
            node_faces[node].insert(face_uid);
          }
          m_cell_face_uids.push_back(face_uid);
          m_face_uids.push_back(face_uid++); // todo parallel
          m_face_nb_nodes.push_back(current_face_nb_nodes);
          m_face_node_uids.addRange(sorted_current_face_nodes);
          m_face_nb_cells.push_back(1);
          m_face_uid_indexes[global_face_index] = face_uid_index++;
          if (is_front_cell) {
            m_face_cell_uids.push_back(NULL_ITEM_UNIQUE_ID);
            m_face_cell_uids.push_back(m_cell_uids[cell_index]);
          }
          else {
            m_face_cell_uids.push_back(m_cell_uids[cell_index]);
            m_face_cell_uids.push_back(NULL_ITEM_UNIQUE_ID);
          }
        }
        else {
          for (auto node : current_face_nodes) {
            node_faces[node].insert(m_face_uids[existing_face_index]);
          }
          m_cell_face_uids.push_back(m_face_uids[existing_face_index]);
          m_face_nb_cells[existing_face_index] += 1;
          m_face_uid_indexes[global_face_index] = existing_face_index;
          // add cell to face cell connectivity
          if (is_front_cell) {
            if (m_face_cell_uids[2 * existing_face_index + 1] != NULL_ITEM_UNIQUE_ID) {
              ARCANE_FATAL("Problem in face orientation, face uid {0}, nodes {1}, same orientation in cell {2} and {3}. Change mesh file.",
                           m_face_uids[existing_face_index],
                           current_face_nodes,
                           m_face_cell_uids[2 * existing_face_index + 1],
                           m_cell_uids[cell_index]);
            }
            m_face_cell_uids[2 * existing_face_index + 1] = m_cell_uids[cell_index];
          }
          else {
            if (m_face_cell_uids[2 * existing_face_index] != NULL_ITEM_UNIQUE_ID) {
              ARCANE_FATAL("Problem in face orientation, face uid {0}, nodes {1}, same orientation in cell {2} and {3}. Change mesh file.",
                           m_face_uids[existing_face_index],
                           current_face_nodes,
                           m_face_cell_uids[2 * existing_face_index],
                           m_cell_uids[cell_index]);
            }
            m_face_cell_uids[2 * existing_face_index] = m_cell_uids[cell_index];
          }
        }
        current_face_nodes.clear();
        sorted_current_face_nodes.clear();
      }
      cell_face_index += m_cell_nb_faces[m_cell_uids[cell_index]];
    }
    // fill node_face_uids and node_nb_faces from node_faces (array form [nb_nodes][nb_connected_faces])
    m_node_nb_faces.resize(m_node_uids.size(), 0);
    _flattenConnectivity(node_faces.constSpan(), m_node_nb_faces, m_node_face_uids);
  }
  std::cout << "================FACE NODES ==============" << std::endl;
  std::copy(m_face_node_uids.begin(), m_face_node_uids.end(), std::ostream_iterator<Int64>(std::cout, " "));
  std::cout << std::endl;
  std::copy(m_face_nb_nodes.begin(), m_face_nb_nodes.end(), std::ostream_iterator<Int64>(std::cout, " "));
  std::cout << std::endl;
  std::copy(m_cell_face_indexes.begin(), m_cell_face_indexes.end(), std::ostream_iterator<Int64>(std::cout, " "));
  std::cout << std::endl;
  return m_face_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
edgeUids()
{
  if (m_edge_uids.empty()) {
    auto vtk_grid = m_vtk_grid_reader->GetOutput();
    m_edge_uids.reserve(2 * vtk_grid->GetNumberOfPoints());
    auto* faces = vtk_grid->GetFaces();
    // This array contains the face info per cells (cf. vtk file)
    // first_cell_nb_faces first_cell_first_face_nb_nodes first_cell_first_face_node_1 ... first_cell_first_face_node_n first_cell_second_face_nb_nodes etc

    if (!faces) {
      ARCANE_FATAL("Mesh {0} is not polyhedral: faces are not defined", m_filename);
    }
    Int64 edge_uid = 0;
    m_edge_node_uids.reserve(2 * m_edge_uids.capacity());
    auto face_info_size = faces->GetNumberOfValues();
    auto cell_index = 0;
    auto global_face_index = 0;
    UniqueArray<std::set<Int64>> edge_cells;
    UniqueArray<Int64UniqueArray> edge_faces;
    edge_cells.reserve(m_edge_uids.capacity());
    edge_faces.reserve(m_edge_uids.capacity());
    m_cell_nb_edges.resize(m_cell_uids.size(), 0);
    m_cell_edge_uids.reserve(20 * m_cell_uids.size()); // choose a value of 20 edge per cell
    UniqueArray<std::set<Int64>> face_edges;
    face_edges.resize(m_face_uids.size());
    UniqueArray<std::set<Int64>> cell_edges;
    cell_edges.resize(m_cell_uids.size());
    UniqueArray<std::set<Int64>> node_edges;
    node_edges.resize(m_node_uids.size());
    for (int face_info_index = 0; face_info_index < face_info_size; ++cell_index) {
      auto current_cell_nb_faces = Int32(faces->GetValue(face_info_index++));
      for (auto face_index = 0; face_index < current_cell_nb_faces; ++face_index, ++global_face_index) {
        auto current_face_nb_nodes = Int32(faces->GetValue(face_info_index++));
        auto first_face_node_uid = Int32(faces->GetValue(face_info_index));
        UniqueArray<Int64> current_edge(2), sorted_edge(2);
        for (int node_index = 0; node_index < current_face_nb_nodes - 1; ++node_index) {
          current_edge = UniqueArray<Int64>{ faces->GetValue(face_info_index++), faces->GetValue(face_info_index) };
          mesh_utils::reorderNodesOfFace(current_edge, sorted_edge); // works for edges
          auto [is_edge_found, existing_edge_index] = _findFace(sorted_edge, m_edge_node_uids, Int32UniqueArray(m_edge_uids.size(), 2)); // works for edges
          if (!is_edge_found) {
            m_cell_nb_edges[cell_index] += 1;
            m_cell_edge_uids.push_back(edge_uid);
            face_edges[m_face_uid_indexes[global_face_index]].insert(edge_uid);
            cell_edges[cell_index].insert(edge_uid);
            for (auto node : current_edge) {
              node_edges[node].insert(edge_uid);
            }
            edge_cells.push_back(std::set{ m_cell_uids[cell_index] });
            edge_faces.push_back(Int64UniqueArray{ m_cell_face_uids[m_cell_face_indexes[cell_index] + face_index] });
            m_edge_uids.push_back(edge_uid++); // todo parallel
            m_edge_node_uids.addRange(sorted_edge);
          }
          else {
            edge_cells[existing_edge_index].insert(m_cell_uids[cell_index]);
            edge_faces[existing_edge_index].push_back(m_cell_face_uids[m_cell_face_indexes[cell_index] + face_index]);
            face_edges[m_face_uid_indexes[global_face_index]].insert(m_edge_uids[existing_edge_index]);
            cell_edges[cell_index].insert(m_edge_uids[existing_edge_index]);
            for (auto node : current_edge) {
              node_edges[node].insert(m_edge_uids[existing_edge_index]);
            }
          }
        }
        current_edge = UniqueArray<Int64>{ faces->GetValue(face_info_index++), first_face_node_uid };
        mesh_utils::reorderNodesOfFace(current_edge, sorted_edge); // works for edges
        auto [is_edge_found, existing_edge_index] = _findFace(sorted_edge, m_edge_node_uids, Int32UniqueArray(m_edge_uids.size(), 2)); // works for edges
        if (!is_edge_found) {
          m_cell_nb_edges[cell_index] += 1;
          m_cell_edge_uids.push_back(edge_uid);
          edge_cells.push_back(std::set{ m_cell_uids[cell_index] });
          edge_faces.push_back(Int64UniqueArray{ m_cell_face_uids[m_cell_face_indexes[cell_index] + face_index] });
          face_edges[m_face_uid_indexes[global_face_index]].insert(edge_uid);
          cell_edges[cell_index].insert(edge_uid);
          for (auto node : current_edge) {
            node_edges[node].insert(edge_uid);
          }
          m_edge_uids.push_back(edge_uid++); // todo parallel
          m_edge_node_uids.addRange(sorted_edge);
        }
        else {
          edge_cells[existing_edge_index].insert(m_cell_uids[cell_index]);
          edge_faces[existing_edge_index].push_back(m_cell_face_uids[m_cell_face_indexes[cell_index] + face_index]);
          face_edges[m_face_uid_indexes[global_face_index]].insert(m_edge_uids[existing_edge_index]);
          cell_edges[cell_index].insert(m_edge_uids[existing_edge_index]);
          for (auto node : current_edge) {
            node_edges[node].insert(m_edge_uids[existing_edge_index]);
          }
        }
      }
    }
    // fill edge_cell_uids and edge_nb_cells from edge_cells (array form [nb_edges][nb_connected_cells])
    m_edge_nb_cells.resize(m_edge_uids.size(), 0);
    _flattenConnectivity(edge_cells.constSpan(), m_edge_nb_cells, m_edge_cell_uids);

    // fill edge faces uids
    m_edge_nb_faces.resize(m_edge_uids.size(), 0);
    _flattenConnectivity(edge_faces.constSpan(), m_edge_nb_faces, m_edge_face_uids);

    // fill face edge uids
    m_face_nb_edges.resize(m_face_uids.size(), 0);
    _flattenConnectivity(face_edges.constSpan(), m_face_nb_edges, m_face_edge_uids);

    // fill cell edge uids
    m_cell_nb_edges.resize(m_cell_uids.size(), 0);
    _flattenConnectivity(cell_edges, m_cell_nb_edges, m_cell_edge_uids);

    // fill node edge uids
    m_node_nb_edges.resize(m_node_uids.size(), 0);
    _flattenConnectivity(node_edges, m_node_nb_edges, m_node_edge_uids);

    // fill edge nb nodes
    m_edge_nb_nodes.resize(m_edge_uids.size(), 2);
  }
  std::cout << "================EDGE NODES ==============" << std::endl;
  std::copy(m_edge_node_uids.begin(), m_edge_node_uids.end(), std::ostream_iterator<Int64>(std::cout, " "));
  std::cout << std::endl;
  std::cout << "================FACE EDGES ==============" << std::endl;
  std::copy(m_face_nb_edges.begin(), m_face_nb_edges.end(), std::ostream_iterator<Int32>(std::cout, " "));
  std::cout << std::endl;
  std::copy(m_face_edge_uids.begin(), m_face_edge_uids.end(), std::ostream_iterator<Int64>(std::cout, " "));
  std::cout << std::endl;
  return m_edge_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

std::pair<bool, Int32> VtkPolyhedralMeshIOService::VtkReader::
_findFace(Int64ConstArrayView face_nodes, Int64ConstArrayView face_node_uids, Int32ConstArrayView face_nb_nodes)
{
  // todo coder l'algo recherche : d'abord on vérifie nombre de noeuds puis on teste tant que l'id est égal (est-ce beaucoup plus rapide ?)
  auto it = std::search(face_node_uids.begin(), face_node_uids.end(), std::boyer_moore_searcher(face_nodes.begin(), face_nodes.end()));
  auto face_index = -1; // 0 starting index
  if (it != face_node_uids.end()) {
    // compute face_index
    auto found_face_position = std::distance(face_node_uids.begin(), it);
    auto position = 0;
    face_index = 0;
    while (position != found_face_position) {
      position += face_nb_nodes[face_index++];
    }
    // compute
  }
  return std::make_pair(it != face_node_uids.end(), face_index);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Integer VtkPolyhedralMeshIOService::VtkReader::
nbNodes()
{
  if (m_node_uids.empty())
    nodeUids();
  return m_node_uids.size();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
cellNodes()
{
  if (m_cell_node_uids.empty())
    cellUids();
  return m_cell_node_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
cellNbNodes()
{
  if (m_cell_nb_nodes.empty())
    cellUids();
  return m_cell_nb_nodes;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
faceNodes()
{
  if (m_face_node_uids.empty())
    faceUids();
  return m_face_node_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
faceNbNodes()
{
  if (m_face_nb_nodes.empty())
    faceUids();
  return m_face_nb_nodes;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
edgeNbNodes()
{
  if (m_edge_node_uids.empty())
    edgeUids();
  return m_edge_nb_nodes;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
edgeNodes()
{
  if (m_edge_node_uids.empty())
    edgeUids();
  return m_edge_node_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
faceCells()
{
  if (m_face_cell_uids.empty())
    faceUids();
  // debug
  std::cout << "=================FACE CELLS================="
            << "\n";
  std::copy(m_face_cell_uids.begin(), m_face_cell_uids.end(), std::ostream_iterator<Int64>(std::cout, " "));
  std::cout << "\n";
  std::cout << "=================END FACE CELLS================="
            << "\n";
  return m_face_cell_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
faceNbCells()
{
  if (m_face_nb_cells.empty())
    faceUids();
  return m_face_nb_cells;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
edgeNbCells()
{
  if (m_edge_nb_cells.empty())
    edgeUids();
  return m_edge_nb_cells;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
edgeCells()
{
  if (m_edge_cell_uids.empty())
    edgeUids();
  return m_edge_cell_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
cellNbFaces()
{
  if (m_cell_nb_faces.empty())
    faceUids();
  return m_cell_nb_faces;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
cellFaces()
{
  if (m_cell_face_uids.empty())
    faceUids();
  return m_cell_face_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
edgeNbFaces()
{
  if (m_edge_nb_faces.empty())
    edgeUids();
  return m_edge_nb_faces;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
edgeFaces()
{
  if (m_edge_face_uids.empty())
    edgeUids();
  return m_edge_face_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
cellNbEdges()
{
  if (m_cell_nb_edges.empty())
    edgeUids();
  return m_cell_nb_edges;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
cellEdges()
{
  if (m_cell_edge_uids.empty())
    edgeUids();
  return m_cell_edge_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
faceNbEdges()
{
  if (m_face_nb_edges.empty())
    edgeUids();
  return m_face_nb_edges;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
faceEdges()
{
  if (m_face_edge_uids.empty())
    edgeUids();
  return m_face_edge_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template <typename Connectivity2DArray>
void VtkPolyhedralMeshIOService::VtkReader::
_flattenConnectivity(Connectivity2DArray connected_item_2darray,
                     Int32Span nb_connected_item_per_source_item,
                     Int64UniqueArray& connected_item_array)
{
  // fill nb_connected_item_per_source_items
  std::transform(connected_item_2darray.begin(), connected_item_2darray.end(), nb_connected_item_per_source_item.begin(), [](auto const& connected_items) {
    return connected_items.size();
  });
  // fill edge_cell_uids
  connected_item_array.reserve(std::accumulate(nb_connected_item_per_source_item.begin(), nb_connected_item_per_source_item.end(), 0));
  std::for_each(connected_item_2darray.begin(), connected_item_2darray.end(), [&connected_item_array](auto const& connected_items) {
    for (auto const& connected_item : connected_items) {
      connected_item_array.push_back(connected_item);
    }
  });
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
nodeNbCells()
{
  if (m_node_nb_cells.empty())
    nodeUids();
  return m_node_nb_cells;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
nodeCells()
{
  if (m_node_cell_uids.empty())
    nodeUids();
  return m_node_cell_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
nodeNbFaces()
{
  if (m_node_nb_faces.empty())
    faceUids();
  return m_node_nb_faces;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
nodeFaces()
{
  if (m_node_face_uids.empty())
    faceUids();
  return m_node_face_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int32ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
nodeNbEdges()
{
  if (m_node_nb_edges.empty())
    edgeUids();
  return m_node_nb_edges;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Int64ConstArrayView VtkPolyhedralMeshIOService::VtkReader::
nodeEdges()
{
  if (m_node_edge_uids.empty())
    edgeUids();
  return m_node_edge_uids;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Real3ArrayView VtkPolyhedralMeshIOService::VtkReader::
nodeCoords()
{
  if (m_node_coordinates.empty()) {
    auto* vtk_grid = m_vtk_grid_reader->GetOutput();
    auto point_coords = vtk_grid->GetPoints()->GetData();
    //    std::cout << "======= Point COORDS ====" << std::endl;
    //    std::ostringstream oss;
    //    point_coords->PrintSelf(oss, vtkIndent{ 2 });
    //    std::cout << oss.str() << std::endl;
    auto nb_nodes = vtk_grid->GetNumberOfPoints();
    for (int i = 0; i < nb_nodes; ++i) {
      //      std::cout << "==========current point coordinates : ( ";
      //      std::cout << *(point_coords->GetTuple(i)) << " , ";
      //      std::cout << *(point_coords->GetTuple(i)+1) << " , ";
      //      std::cout << *(point_coords->GetTuple(i)+2) << " ) ===" << std::endl;
      m_node_coordinates.add({ *(point_coords->GetTuple(i)),
                               *(point_coords->GetTuple(i) + 1),
                               *(point_coords->GetTuple(i) + 2) });
    }
  }
  return m_node_coordinates;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane