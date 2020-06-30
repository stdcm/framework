//
// Created by dechaiss on 5/6/20.
//

#ifndef NEO_MESH_H
#define NEO_MESH_H

/*-------------------------
 * sdc - (C) 2020
 * NEtwork Oriented kernel
 * POC Mesh API
 *--------------------------
 */

#include <memory>
#include <string>
#include <vector>

#include "Neo.h"
#include "neo/Utils.h"

namespace Neo {

class MeshBase;
enum class ItemKind;
class Family;
struct ScheduledItemRange;
class ItemRangeUnlocker;

class Mesh {

public:
  using UidPropertyType   = Neo::PropertyT<Neo::utils::Int64>;
  using CoordPropertyType = Neo::PropertyT<Neo::utils::Real3>;

public:
  Mesh(std::string const& mesh_name);
  ~Mesh();

private:
  std::unique_ptr<MeshBase> m_mesh_graph;

public:
  [[nodiscard]] std::string const& name() const noexcept ;

  [[nodiscard]] std::string uniqueIdPropertyName(const std::string& family_name) const noexcept ;

  Neo::Family&  addFamily(Neo::ItemKind item_kind, std::string family_name) noexcept ;

  void scheduleAddItems(Neo::Family& family, std::vector<Neo::utils::Int64> const& uids, Neo::ScheduledItemRange & future_added_item_range) noexcept ;
  void scheduleAddItems(Neo::Family& family, std::vector<Neo::utils::Int64> && uids, Neo::ScheduledItemRange & future_added_item_range) noexcept ;

  //! Connectivity vith fix size (nb of connected items per item is constant)
  //! Use this method to add connectivity of new items (i.e addItems and addConnectivity are applied with the same call to applyScheduledOperations)
  void scheduleAddConnectivity(Neo::Family& source_family, Neo::ScheduledItemRange const& source_items,
                               Neo::Family& target_family, int nb_connected_item_per_item,
                               std::vector<Neo::utils::Int64>& connected_item_uids,
                               std::string const& connectivity_name) noexcept ;

  //! Use this method to add connectivity of existing items
  void scheduleAddConnectivity(Neo::Family& source_family, Neo::ItemRange const& source_items,
                               Neo::Family& target_family, int nb_connected_item_per_item,
                               std::vector<Neo::utils::Int64>& connected_item_uids,
                               std::string const& connectivity_name) noexcept ;

  //! Connectivity vith variable size (nb of connected items per item is variable)
  //! Use this method to add connectivity of new items (i.e addItems and addConnectivity are applied with the same call to applyScheduledOperations)
  void scheduleAddConnectivity(Neo::Family& source_family, Neo::ScheduledItemRange const& source_items,
                               Neo::Family& target_family, std::vector<int> nb_connected_item_per_item,
                               std::vector<Neo::utils::Int64>& connected_item_uids,
                               std::string const& connectivity_name) noexcept ;

  //! Use this method to add connectivity of existing items
  void scheduleAddConnectivity(Neo::Family& source_family, Neo::ItemRange const& source_items,
                               Neo::Family& target_family, std::vector<int> nb_connected_item_per_item,
                               std::vector<Neo::utils::Int64>& connected_item_uids,
                               std::string const& connectivity_name) noexcept ;

  //! Use this method to set coordinates of new items
  void scheduleSetItemCoords(Neo::Family& item_family, Neo::ScheduledItemRange const& future_added_item_range,std::vector<Neo::utils::Real3> const& item_coords) noexcept ;
  void scheduleSetItemCoords(Neo::Family& item_family, Neo::ScheduledItemRange const& future_added_item_range,std::vector<Neo::utils::Real3> && item_coords) noexcept ;

  Neo::ItemRangeUnlocker applyScheduledOperations() noexcept ;

  //! Use this method to change coordinates of existing items
  [[nodiscard]] CoordPropertyType& getItemCoordProperty(Neo::Family & family);
  [[nodiscard]] CoordPropertyType const& getItemCoordProperty(Neo::Family const& family) const;

private:
  [[nodiscard]] std::string _itemCoordPropertyName(Family const& item_family) const {return item_family.name()+"_item_coordinates";}
};

} // end namespace Neo

#endif // NEO_MESH_H
