﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* Item.h                                                      (C) 2000-2022 */
/*                                                                           */
/* Informations sur les éléments du maillage.                                */
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_ITEM_H
#define ARCANE_ITEM_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/ItemTypes.h"
#include "arcane/ItemInternal.h"
#include "arcane/ItemLocalId.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Macro pour vérifier en mode Check que les conversions entre les
// les genres d'entités sont correctes.
#ifdef ARCANE_CHECK
#define ARCANE_CHECK_KIND(type) _checkKind(type())
#else
#define ARCANE_CHECK_KIND(type)
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Classe de base d'un élément de maillage.
 *
 * \ingroup Mesh

 Les éléments du maillage sont les noeuds (Node), les mailles (Cell),
 les faces (Face) et les arêtes (Edge). Chacun de ses éléments est décrit
 dans la classe dérivée correspondante.

 Quel que soit son type un élément du maillage possède un identifiant
 unique (localId()) pour son type et local au sous-domaine géré et un identifiant
 unique (uniqueId()) pour son type sur l'ensemble du domaine. La numérotation est
 <b>continue</b> et commence à <b>0</b>. L'identifiant local est utilisé par
 exemple pour accéder aux variables ou pour la connectivité.

 Par exemple, si un maillage possède 2 mailles hexaédriques qui se joignent
 par une face, il y 12 noeuds, 11 faces et 2 mailles. Dans ce cas, le premier
 noeud aura l'identifiant 0, le second 1 et ainsi de suite jusqu'à 11. La
 première face aura l'identifiant 0, la seconde 1 et ainsi de suite
 jusqu'à 10.

 Cette classe contient aussi les déclarations des types des éléments supportés
 des types d'éléments de maillage supportés (KindType) et
 des constantes comme le nombre maximum de sommets dans une face. Ces
 constantes peuvent être utilisées quand on doit définir des tableaux
 qui conviennent quel que soit le type de maille par exemple.

 Il existe une entité de correspondant à un objet nul. C'est la seul
 pour laquelle null() est vrai. Aucune opération autre que l'appel à null()
 et les opérations de comparaisons ne sont valident sur l'entitée nulle.
 */
class ARCANE_CORE_EXPORT Item
: protected ItemBase
{
 public:

  typedef ItemInternal* ItemInternalPtr;
  
  //! Type du localId()
  typedef ItemLocalId LocalIdType;

 public:

  /*!
   * \brief Index d'un Item dans une variable.
   * \deprecated
   */
  class Index
  {
   public:
    Index() : m_local_id(NULL_ITEM_LOCAL_ID){}
    explicit Index(Int32 id) : m_local_id(id){}
    Index(Item item) : m_local_id(item.localId()){}
    operator ItemLocalId() const { return ItemLocalId{m_local_id}; }
   public:
    Int32 localId() const { return m_local_id; }
   private:
    Int32 m_local_id;
  };

 public:

  /*!
   * \brief Type des éléments.
   *
   * Les valeurs des types doivent aller de 0 à #NB_TYPE par pas de 1.
   *
   * \deprecated. Utilise les types définis dans ArcaneTypes.h
   */
  enum
  {
    Unknown ARCANE_DEPRECATED_REASON("Use 'IT_NullType' instead") = IT_NullType, //!< Elément de type nul
    Vertex ARCANE_DEPRECATED_REASON("Use 'IT_Vertex' instead") = IT_Vertex, //!< Elément de type noeud (1 sommet 1D, 2D et 3D)
    Bar2 ARCANE_DEPRECATED_REASON("Use 'IT_Line2' instead") = IT_Line2, //!< Elément de type arête (2 sommets, 1D, 2D et 3D)
    Tri3 ARCANE_DEPRECATED_REASON("Use 'IT_Triangle3' instead") = IT_Triangle3, //!< Elément de type triangle (3 sommets, 2D)
    Quad4 ARCANE_DEPRECATED_REASON("Use 'IT_Quad4' instead") = IT_Quad4, //!< Elément de type quad (4 sommets, 2D)
    Pentagon5 ARCANE_DEPRECATED_REASON("Use 'IT_Pentagon5' instead") = IT_Pentagon5, //!< Elément de type pentagone (5 sommets, 2D)
    Hexagon6 ARCANE_DEPRECATED_REASON("Use 'IT_Hexagon6' instead") = IT_Hexagon6, //!< Elément de type hexagone (6 sommets, 2D)
    Tetra ARCANE_DEPRECATED_REASON("Use 'IT_Tetraedron4' instead") = IT_Tetraedron4, //!< Elément de type tétraédre (4 sommets, 3D)
    Pyramid ARCANE_DEPRECATED_REASON("Use 'IT_Pyramid5' instead") = IT_Pyramid5, //!< Elément de type pyramide  (5 sommets, 3D)
    Penta ARCANE_DEPRECATED_REASON("Use 'IT_Pentaedron6' instead") = IT_Pentaedron6, //!< Elément de type pentaèdre (6 sommets, 3D)
    Hexa ARCANE_DEPRECATED_REASON("Use 'IT_Hexaedron8' instead") = IT_Hexaedron8, //!< Elément de type hexaèdre  (8 sommets, 3D)
    Wedge7 ARCANE_DEPRECATED_REASON("Use 'IT_Heptaedron10' instead") = IT_Heptaedron10, //!< Elément de type prisme à 7 faces (base pentagonale)
    Wedge8 ARCANE_DEPRECATED_REASON("Use 'IT_Octaedron12' instead") = IT_Octaedron12 //!< Elément de type prisme à 8 faces (base hexagonale)
    // Réduit au minimum pour compatibilité.
  };

  //! Indice d'un élément nul
  static const Int32 NULL_ELEMENT = NULL_ITEM_ID;

  //! Nom du type de maille \a cell_type
  ARCCORE_DEPRECATED_2021("Use ItemTypeMng::typeName() instead")
  static String typeName(Int32 type);

 public:

  //! Création d'une entité de maillage nulle
  Item() {}

  //! Constructeur de recopie
  Item(const Item& rhs) : ItemBase(rhs) {}

  //! Construit une référence à l'entité \a internal
  Item(ItemInternal* ainternal) : ItemBase(ainternal) {}

  //! Construit une référence à l'entité \a abase
  Item(const ItemBase& abase) : ItemBase(abase) {}

  //! Construit une référence à l'entité \a internal
  Item(const ItemInternalPtr* internals,Int32 local_id) : ItemBase(internals[local_id]) {}

  //! Opérateur de copie
  Item& operator=(ItemInternal* ainternal)
  {
    _set(ainternal);
    return (*this);
  }

  //! Opérateur de copie
  Item& operator=(const Item& from)
  {
    _set(from);
    return (*this);
  }

 public:

  //! \a true si l'entité est nul (i.e. non connecté au maillage)
  bool null() const { return m_local_id==NULL_ITEM_ID; }

  //! Identifiant local de l'entité dans le sous-domaine du processeur
  Int32 localId() const { return m_local_id; }

  //! Identifiant local de l'entité dans le sous-domaine du processeur
  ItemLocalId itemLocalId() const { return ItemLocalId{ m_local_id }; }

  //! Identifiant unique sur tous les domaines
  ItemUniqueId uniqueId() const { return ItemBase::uniqueId(); }

  //! Numéro du sous-domaine propriétaire de l'entité
  Int32 owner() const { return ItemBase::owner(); }

  //! Type de l'entité
  Int16 type() const { return ItemBase::typeId(); }

  //! Type de l'entité
  ItemTypeId itemTypeId() const { return ItemBase::itemTypeId(); }

  //! Genre de l'entité
  eItemKind kind() const { return ItemBase::kind(); }

  //! \a true si l'entité est appartient au sous-domaine
  bool isOwn() const { return ItemBase::isOwn(); }

  /*!
   * \brief Vrai si l'entité est partagé d'autres sous-domaines.
   *
   * Une entité est considérée comme partagée si et seulement si
   * isOwn() est vrai et elle est fantôme pour un ou plusieurs
   * autres sous-domaines.
   *
   * Cette méthode n'est pertinente que si les informations de connectivité
   * ont été calculées (par un appel à IItemFamily::computeSynchronizeInfos()).
   */
  bool isShared() const { return ItemBase::isShared(); }

  //! Converti l'entité en le genre \a ItemWithNodes.
  inline ItemWithNodes toItemWithNodes() const;
  //! Converti l'entité en le genre \a Node.
  inline Node toNode() const;
  //! Converti l'entité en le genre \a Cell.
  inline Cell toCell() const;
  //! Converti l'entité en le genre \a Edge.
  inline Edge toEdge() const;
  //! Converti l'entité en le genre \a Edge.
  inline Face toFace() const;
  //! Converti l'entité en le genre \a Particle.
  inline Particle toParticle() const;
  //! Converti l'entité en le genre \a DoF.
  inline DoF toDoF() const;

  //! Nombre de parents
  Int32 nbParent() const { return ItemBase::nbParent(); }

  //! i-ème parent
  Item parent(Int32 i) const { return ItemBase::parentBase(i); }

  //! premier parent
  Item parent() const { return ItemBase::parentBase(0); }

 public:

  //! \a true si l'entité est du genre \a ItemWithNodes.
  bool isItemWithNodes() const
  {
    eItemKind ik = kind();
    return (ik==IK_Unknown || ik==IK_Edge || ik==IK_Face || ik==IK_Cell );
  }

  //! \a true si l'entité est du genre \a Node.
  bool isNode() const
  {
    eItemKind ik = kind();
    return (ik==IK_Unknown || ik==IK_Node);
  }
  //! \a true si l'entité est du genre \a Cell.
  bool isCell() const
  {
    eItemKind ik = kind();
    return (ik==IK_Unknown || ik==IK_Cell);
  }
  //! \a true si l'entité est du genre \a Edge.
  bool isEdge() const
  {
    eItemKind ik = kind();
    return (ik==IK_Unknown || ik==IK_Edge);
  }
  //! \a true si l'entité est du genre \a Edge.
  bool isFace() const
  {
    eItemKind ik = kind();
    return (ik==IK_Unknown || ik==IK_Face);
  }
  //! \a true is l'entité est du genre \a Particle.
  bool isParticle() const
  {
    eItemKind ik = kind();
    return (ik==IK_Unknown || ik==IK_Particle);
  }
  //! \a true is l'entité est du genre \a DoF
  bool isDoF() const
  {
    eItemKind ik = kind();
    return (ik==IK_Unknown || ik==IK_DoF);
  }

 public:

  /*!
   * \brief Partie interne de l'entité.
   *
   * \warning La partie interne de l'entité ne doit être modifiée que
   * par ceux qui savent ce qu'ils font.
   */
  ItemInternal* internal() const { return ItemBase::itemInternal(); }

  /*!
   * \brief Infos sur le type de l'entité.
   *
   * Cette méthode permet d'obtenir les informations concernant
   * un type donné d'entité , comme par exemple les numérotations locales
   * de ces faces ou de ses arêtes.
   */
  const ItemTypeInfo* typeInfo() const { return ItemBase::typeInfo(); }

 public:

  Item* operator->() { return this; }
  const Item* operator->() const { return this; }

 protected:

  using ItemBase::m_local_id;

 protected:

  void _checkKind(bool is_valid) const
  {
    if (!is_valid)
      _badConversion();
  }
  void _badConversion() const;
  void _set(ItemInternal* ainternal)
  {
    _setFromInternal(ainternal);
  }
  void _set(const Item& rhs)
  {
    _setFromInternal(rhs);
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Compare deux entités.
 *
 * \retval true si elles sont identiques (mêmes localId())
 * \retval false sinon
 */
inline bool
operator==(const Item& item1,const Item& item2)
{
  return item1.localId()==item2.localId();
}

/*!
 * \brief Compare deux entités.
 *
 * \retval true si elles sont différentes (différents localId())
 * \retval false sinon
 */
inline bool
operator!=(const Item& item1,const Item& item2)
{
  return item1.localId()!=item2.localId();
}

/*!
 * \brief Compare deux entités.
 *
 * \retval true si elles sont inferieures (sur localId())
 * \retval false sinon
 */
inline bool
operator<(const Item& item1,const Item& item2)
{
  return item1.localId()<item2.localId();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/ItemVectorView.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Noeud d'un maillage.
 *
 * \ingroup Mesh
 */
class ARCANE_CORE_EXPORT Node
: public Item
{
 public:
  /*!
   * \brief Index d'un Node dans une variable.
   * \deprecated
   */
  class Index
  : public Item::Index
  {
   public:
    typedef Item::Index Base;
   public:
    explicit Index(Int32 id) : Base(id){}
    Index(Node item) : Base(item){}
    operator NodeLocalId() const { return NodeLocalId{localId()}; }
  };
 public:
  //! Type du localId()
  typedef NodeLocalId LocalIdType;

  //! Création d'un noeud non connecté au maillage
  Node() : Item() {}

  //! Constructeur de recopie
  Node(const Node& rhs) : Item(rhs) {}

  //! Construit une référence à l'entité \a internal
  Node(ItemInternal* ainternal) : Item(ainternal)
  { ARCANE_CHECK_KIND(isNode); }

  //! Construit une référence à l'entité \a abase
  Node(ItemBase abase) : Item(abase)
  { ARCANE_CHECK_KIND(isNode); }

  //! Construit une référence à l'entité \a internal
  Node(const ItemInternalPtr* internals,Int32 local_id) : Item(internals,local_id)
  { ARCANE_CHECK_KIND(isNode); }

  //! Opérateur de copie
  Node& operator=(ItemInternal* ainternal)
  {
    _set(ainternal);
    return (*this);
  }

  //! Opérateur de copie
  Node& operator=(const Node& from)
  {
    _set(from);
    return (*this);
  }

 public:

  //! Identifiant local de l'entité dans le sous-domaine du processeur
  NodeLocalId itemLocalId() const { return NodeLocalId{ m_local_id }; }

  //! Nombre d'arêtes connectées au noeud
  Int32 nbEdge() const { return ItemBase::nbEdge(); }

  //! Nombre de faces connectées au noeud
  Int32 nbFace() const { return ItemBase::nbFace(); }

  //! Nombre de mailles connectées au noeud
  Int32 nbCell() const { return ItemBase::nbCell(); }

  //! i-ème arête du noeud
  inline Edge edge(Int32 i) const;

  //! i-ème face du noeud
  inline Face face(Int32 i) const;

  //! i-ème maille du noeud
  inline Cell cell(Int32 i) const;

  //! i-ème arête du noeud
  EdgeLocalId edgeId(Int32 i) const { return EdgeLocalId(ItemBase::edgeId(i)); }

  //! i-ème face du noeud
  FaceLocalId faceId(Int32 i) const { return FaceLocalId(ItemBase::faceId(i)); }

  //! i-ème maille du noeud
  CellLocalId cellId(Int32 i) const { return CellLocalId(ItemBase::cellId(i)); }

  //! Liste des arêtes du noeud
  EdgeVectorView edges() const { return ItemBase::internalEdges(); }

  //! Liste des faces du noeud
  FaceVectorView faces() const { return ItemBase::internalFaces(); }

  //! Liste des mailles du noeud
  CellVectorView cells() const { return ItemBase::internalCells(); }

  //! Liste des arêtes du noeud
  EdgeLocalIdView edgeIds() const
  {
    return EdgeLocalIdView::fromIds(ItemBase::edgeIds());
  }

  //! Liste des faces du noeud
  FaceLocalIdView faceIds() const
  {
    return FaceLocalIdView::fromIds(ItemBase::faceIds());
  }

  //! Liste des mailles du noeud
  CellLocalIdView cellIds() const
  {
    return CellLocalIdView::fromIds(ItemBase::cellIds());
  }

  // AMR

  //! Enumére les mailles connectées au noeud
  ItemVectorView activeCells(Int32Array& local_ids) const
  {
    return ItemBase::activeCells(local_ids);
  }

  //! Enumére les faces connectées au noeud
  FaceVectorView activeFaces(Int32Array& local_ids) const
  {
    return ItemBase::activeFaces(local_ids);
  }

  //! Enumére les arêtes connectées au noeud
  EdgeVectorView activeEdges() const
  {
    return ItemBase::activeEdges();
  }

  // TODO: a supprimer
  Node* operator->() { return this; }
  // TODO: a supprimer
  const Node* operator->() const { return this; }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Elément de maillage s'appuyant sur des noeuds (Edge,Face,Cell).
 *
 * \ingroup Mesh
 */
class ARCANE_CORE_EXPORT ItemWithNodes
: public Item
{
 public:
  
  //! Création d'une entité non connectée au maillage
  ItemWithNodes() : Item() {}

  //! Constructeur de recopie
  ItemWithNodes(const ItemWithNodes& rhs) : Item(rhs) {}

  //! Construit une référence à l'entité \a internal
  ItemWithNodes(ItemInternal* ainternal) : Item(ainternal)
  { ARCANE_CHECK_KIND(isItemWithNodes); }

  //! Construit une référence à l'entité \a abase
  ItemWithNodes(const ItemBase& abase) : Item(abase)
  { ARCANE_CHECK_KIND(isItemWithNodes); }

  //! Construit une référence à l'entité \a internal
  ItemWithNodes(const ItemInternalPtr* internals,Int32 local_id)
  : Item(internals,local_id)
  { ARCANE_CHECK_KIND(isItemWithNodes); }

  //! Opérateur de copie
  ItemWithNodes& operator=(ItemInternal* ainternal)
  {
    _set(ainternal);
    return (*this);
  }

  //! Opérateur de copie
  ItemWithNodes& operator=(const ItemWithNodes& from)
  {
    _set(from);
    return (*this);
  }

 public:

  //! Nombre de noeuds de l'entité
  Int32 nbNode() const { return ItemBase::nbNode(); }

  //! i-ème noeud de l'entité
  Node node(Int32 i) const { return Node(ItemBase::nodeBase(i)); }

  //! Liste des noeuds de l'entité
  NodeVectorView nodes() const { return ItemBase::internalNodes(); }

  //! Liste des noeuds de l'entité
  NodeLocalIdView nodeIds() const
  {
    return NodeLocalIdView::fromIds(ItemBase::nodeIds());
  }

  //! i-ème noeud de l'entité.
  NodeLocalId nodeId(Int32 index) const
  {
    return NodeLocalId(ItemBase::nodeId(index));
  }

  ItemWithNodes* operator->() { return this; }
  const ItemWithNodes* operator->() const { return this; }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Arête d'une maille.
 *
 * Les arêtes n'existent qu'en 3D. En 2D, il faut utiliser la structure
 * 'Face'.
 *
 * \ingroup Mesh
 */
class ARCANE_CORE_EXPORT Edge
: public ItemWithNodes
{
 public:
  /*!
   * \brief Index d'une Edge dans une variable.
   * \deprecated
   */
  class Index
  : public Item::Index
  {
   public:
    typedef Item::Index Base;
   public:
    explicit Index(Int32 id) : Base(id){}
    Index(Edge item) : Base(item){}
    operator EdgeLocalId() const { return EdgeLocalId{localId()}; }
  };
 public:
  //! Type du localId()
  typedef EdgeLocalId LocalIdType;

  //! Créé une arête nulle
  Edge() : ItemWithNodes() {}

  //! Constructeur de recopie
  Edge(const Edge& rhs) : ItemWithNodes(rhs) {}

  //! Construit une référence à l'entité \a internal
  Edge(ItemInternal* ainternal) : ItemWithNodes(ainternal)
  { ARCANE_CHECK_KIND(isEdge); }

  //! Construit une référence à l'entité \a abase
  Edge(const ItemBase& abase) : ItemWithNodes(abase)
  { ARCANE_CHECK_KIND(isEdge); }

  //! Construit une référence à l'entité \a internal
  Edge(const ItemInternalPtr* internals,Int32 local_id)
  : ItemWithNodes(internals,local_id)
  { ARCANE_CHECK_KIND(isEdge); }

  //! Opérateur de copie
  Edge& operator=(ItemInternal* ainternal)
  {
    _set(ainternal);
    return (*this);
  }

  //! Opérateur de copie
  Edge& operator=(const Edge& from)
  {
    _set(from);
    return (*this);
  }

 public:

  //! Identifiant local de l'entité dans le sous-domaine du processeur
  EdgeLocalId itemLocalId() const { return EdgeLocalId{ m_local_id }; }

  //! Nombre de sommets de l'arête
  Int32 nbNode() const { return 2; }

  //! Nombre de faces connectées à l'arête
  Int32 nbFace() const { return ItemBase::nbFace(); }

  //! Nombre de mailles connectées à l'arête
  Int32 nbCell() const { return ItemBase::nbCell(); }

  //! i-ème maille de l'arête
  inline Cell cell(Int32 i) const;

  //! Liste des mailles de l'arête
  CellVectorView cells() const { return ItemBase::internalCells(); }

  //! i-ème maille de l'arête
  CellLocalId cellId(Int32 i) const { return CellLocalId(ItemBase::cellId(i)); }

  //! Liste des mailles de l'arête
  CellLocalIdView cellIds() const
  {
    return CellLocalIdView::fromIds(ItemBase::cellIds());
  }

  //! i-ème face de l'arête
  inline Face face(Int32 i) const;

  //! Liste des faces de l'arête
  FaceVectorView faces() const { return ItemBase::internalFaces(); }

  //! i-ème face de l'arête
  FaceLocalId faceId(Int32 i) const { return FaceLocalId(ItemBase::faceId(i)); }

  //! Liste des faces de l'arête
  FaceLocalIdView faceIds() const
  {
    return FaceLocalIdView::fromIds(ItemBase::faceIds());
  }

  Edge* operator->() { return this; }
  const Edge* operator->() const { return this; }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Face d'une maille.
 *
 * \ingroup Mesh
 *
 Une face est décrite par la liste ordonnée de ses sommets, ce qui lui
 donne une orientation.
 */
class ARCANE_CORE_EXPORT Face
: public ItemWithNodes
{
 public:
  /*!
   * \brief Index d'une Face dans une variable.
   * \deprecated
   */
  class Index
  : public Item::Index
  {
   public:
    typedef Item::Index Base;
   public:
    explicit Index(Int32 id) : Base(id){}
    Index(Face item) : Base(item){}
    operator FaceLocalId() const { return FaceLocalId{localId()}; }
  };

 public:
  //! Type du localId()
  typedef FaceLocalId LocalIdType;

  //! Création d'une face non connecté au maillage
  Face() : ItemWithNodes() {}

  //! Constructeur de recopie
  Face(const Face& rhs) : ItemWithNodes(rhs) {}

  //! Construit une référence à l'entité \a internal
  Face(ItemInternal* ainternal) : ItemWithNodes(ainternal)
  { ARCANE_CHECK_KIND(isFace); }

  //! Construit une référence à l'entité \a abase
  Face(ItemBase abase) : ItemWithNodes(abase)
  { ARCANE_CHECK_KIND(isFace); }

  //! Construit une référence à l'entité \a internal
  Face(const ItemInternalPtr* internals,Int32 local_id)
  : ItemWithNodes(internals,local_id)
  { ARCANE_CHECK_KIND(isFace); }

  //! Opérateur de copie
  Face& operator=(ItemInternal* ainternal)
  {
    _set(ainternal);
    return (*this);
  }

  //! Opérateur de copie
  Face& operator=(const Face& from)
  {
    _set(from);
    return (*this);
  }

 public:

  //! Identifiant local de l'entité dans le sous-domaine du processeur
  FaceLocalId itemLocalId() const { return FaceLocalId{ m_local_id }; }

  //! Nombre de mailles de la face (1 ou 2)
  Int32 nbCell() const { return ItemBase::nbCell(); }

  //! i-ème maille de la face
  inline Cell cell(Int32 i) const;

  //! Liste des mailles de la face
  CellVectorView cells() const { return ItemBase::internalCells(); }

  //! i-ème maille de la face
  CellLocalId cellId(Int32 i) const { return CellLocalId(ItemBase::cellId(i)); }

  //! Liste des mailles de la face
  CellLocalIdView cellIds() const
  {
    return CellLocalIdView::fromIds(ItemBase::cellIds());
  }

  /*!
   * \brief Indique si la face est au bord du sous-domaine (i.e nbCell()==1)
   *
   * \warning Une face au bord du sous-domaine n'est pas nécessairement au bord du maillage global.
   */
  bool isSubDomainBoundary() const { return ItemBase::isBoundary(); }

  /*!
   * \a true si la face est au bord du sous-domaine.
   * \deprecated Utiliser isSubDomainBoundary() à la place.
   */
  ARCANE_DEPRECATED_118 bool isBoundary() const { return ItemBase::isBoundary(); }

  //! Indique si la face est au bord t orientée vers l'extérieur.
  bool isSubDomainBoundaryOutside() const
  {
    return isSubDomainBoundary() && (ItemBase::flags() & ItemInternal::II_HasBackCell);
  }

  /*!
   * \brief Indique si la face est au bord t orientée vers l'extérieur.
   *
   * \deprecated Utiliser isSubDomainBoundaryOutside()
   */
  ARCANE_DEPRECATED_118 bool isBoundaryOutside() const
  {
    return isSubDomainBoundaryOutside();
  }

  //! Maille associée à cette face frontière (maille nulle si aucune)
  inline Cell boundaryCell() const;

  //! Maille derrière la face (maille nulle si aucune)
  inline Cell backCell() const;

  //! Maille derrière la face (maille nulle si aucune)
  CellLocalId backCellId() const { return CellLocalId{ItemBase::backCellId()}; }

  //! Maille devant la face (maille nulle si aucune)
  inline Cell frontCell() const;

  //! Maille devant la face (maille nulle si aucune)
  CellLocalId frontCellId() const { return CellLocalId{ItemBase::frontCellId()}; }

  /*!
   * \brief Maille opposée de cette face à la maille \a cell.
   *
   * \pre backCell()==cell || frontCell()==cell.
   */
  inline Cell oppositeCell(Cell cell) const;

  /*!
   * \brief Maille opposée de cette face à la maille \a cell.
   *
   * \pre backCell()==cell || frontCell()==cell.
   */
  CellLocalId oppositeCellId(CellLocalId cell_id) const
  {
    ARCANE_ASSERT((backCellId()==cell_id || frontCellId()==cell_id),("cell is not connected to the face"));
    return (backCellId()==cell_id) ? frontCellId() : backCellId();
  }

  /*!
   * \brief Face maître associée à cette face.
   *
   * Cette face n'est non nul que si la face est liée à une interface
   * et est une face esclave de cette interface (i.e. isSlaveFace() est vrai)
   *
   * \sa ITiedInterface
   */
  Face masterFace() const { return ItemBase::masterFace(); }

  //! \a true s'il s'agit de la face maître d'une interface
  bool isMasterFace() const { return ItemBase::isMasterFace(); }

  //! \a true s'il s'agit d'une face esclave d'une interface
  bool isSlaveFace() const { return ItemBase::isSlaveFace(); }

  //! \a true s'il s'agit d'une face esclave ou maître d'une interface
  bool isTiedFace() const { return isSlaveFace() || isMasterFace(); }

  /*!
   * \brief Liste des faces esclaves associées à cette face maître.
   *
   * Cette liste n'existe que pour les faces dont isMasterFace() est vrai.
   * Pour les autres, elle est vide.
   */
  FaceVectorView slaveFaces() const
  {
    if (ItemBase::isMasterFace())
      return ItemBase::internalFaces();
    return FaceVectorView();
  }

 public:

  //! Nombre d'arêtes de la face
  Int32 nbEdge() const { return ItemBase::nbEdge(); }

  //! i-ème arête de la face
  Edge edge(Int32 i) const { return Edge(ItemBase::edgeBase(i)); }

  //! Liste des arêtes de la face
  EdgeVectorView edges() const { return ItemBase::internalEdges(); }

  //! i-ème arête de la face
  EdgeLocalId edgeId(Int32 i) const { return EdgeLocalId(ItemBase::edgeId(i)); }

  //! Liste des arêtes de la face
  EdgeLocalIdView edgeIds() const
  {
    return EdgeLocalIdView::fromIds(ItemBase::edgeIds());
  }

  Face* operator->() { return this; }
  const Face* operator->() const { return this; }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Maille d'un maillage.
 *
 * \ingroup Mesh
 *
 Chaque maille utilise de la mémoire pour stocker sa connectivité. Cela
 permet aux modules d'écrire leur boucle de manière identique quelle que
 soit le type de la maille. Dans un premier temps, c'est le mécanisme le
 plus simple. On peut envisager par la suite d'utiliser des classes template
 pour traiter la même information de manière statique (i.e. toute la connectivité
 est gérée à la compilation).

 La connectivité utilise la numérotation <b>locale</b> des sommets de
 la maille. Elle est stockée dans les variables de classe #global_face_list
 pour les faces et #global_edge_list pour les arêtes.

 La connectivité utilisée est celle qui est décrite dans la notice
 LIMA version 3.1 à ceci près que la numérotation commence à zéro et non
 pas à un.

 LIMA ne décrivant pas la pyramide, la numérotation utilisée est celle
 de l'hexaèdre dégénérée en considérant que les sommets 4, 5, 6 et 7
 sont le sommet de la pyramide

 Dans la version actuelle (1.6), les arêtes ne sont pas prises en compte
 de manière globale (i.e: il n'y a pas d'entités Edge par maille).
*/
class ARCANE_CORE_EXPORT Cell
: public ItemWithNodes
{
 public:
  /*!
   * \brief Index d'une Cell dans une variable.
   * \deprecated
   */
  class Index
  : public Item::Index
  {
   public:
    typedef Item::Index Base;
   public:
    explicit Index(Int32 id) : Base(id){}
    Index(Cell item) : Base(item){}
    operator CellLocalId() const { return CellLocalId{localId()}; }
  };
 public:
  //! Type du localId()
  typedef CellLocalId LocalIdType;

  //! Constructeur d'une maille nulle
  Cell() : ItemWithNodes() {}

  //! Constructeur de recopie
  Cell(const Cell& rhs) : ItemWithNodes(rhs) {}

  //! Construit une référence à l'entité \a internal
  Cell(ItemInternal* ainternal) : ItemWithNodes(ainternal)
  { ARCANE_CHECK_KIND(isCell); }

  //! Construit une référence à l'entité \a abase
  Cell(ItemBase abase) : ItemWithNodes(abase)
  { ARCANE_CHECK_KIND(isCell); }

  //! Construit une référence à l'entité \a internal
  Cell(const ItemInternalPtr* internals,Int32 local_id)
  : ItemWithNodes(internals,local_id)
  { ARCANE_CHECK_KIND(isCell); }

  //! Opérateur de copie
  Cell& operator=(ItemInternal* ainternal)
  {
    _set(ainternal);
    return (*this);
  }

  //! Opérateur de copie
  Cell& operator=(const Cell& from)
  {
    _set(from);
    return (*this);
  }

 public:

  //! Identifiant local de l'entité dans le sous-domaine du processeur
  CellLocalId itemLocalId() const { return CellLocalId{ m_local_id }; }

  //! Nombre de faces de la maille
  Int32 nbFace() const { return ItemBase::nbFace(); }

  //! i-ème face de la maille
  Face face(Int32 i) const { return Face(ItemBase::faceBase(i)); }

  //! Liste des faces de la maille
  FaceVectorView faces() const { return ItemBase::internalFaces(); }

  //! i-ème face de la maille
  FaceLocalId faceId(Int32 i) const { return FaceLocalId(ItemBase::faceId(i)); }

  //! Liste des faces de la maille
  FaceLocalIdView faceIds() const
  {
    return FaceLocalIdView::fromIds(ItemBase::faceIds());
  }

  //! Nombre d'arêtes de la maille
  Int32 nbEdge() const { return ItemBase::nbEdge(); }

  //! i-ème arête de la maille
  Edge edge(Int32 i) const { return Edge(ItemBase::edgeBase(i)); }

  //! i-ème arête de la maille
  EdgeLocalId edgeId(Int32 i) const { return EdgeLocalId(ItemBase::edgeId(i)); }

  //! Liste des arêtes de la maille
  EdgeVectorView edges() const { return ItemBase::internalEdges(); }

  //! Liste des arêtes de la maille
  EdgeLocalIdView edgeIds() const
  {
    return EdgeLocalIdView::fromIds(ItemBase::edgeIds());
  }

  //! AMR
  //! ATT: la notion de parent est utilisé à la fois dans le concept sous-maillages et AMR.
  //! La première implémentation AMR sépare les deux concepts pour des raisons de consistances.
  //! Une fusion des deux notions est envisageable dans un deuxième temps
  //! dans un premier temps, les appelations, pour l'amr, sont en français i.e. parent -> pere et child -> enfant
  //! un seul parent
  Cell hParent() const { return Cell(ItemBase::hParentBase(0)); }

  Int32 nbHChildren() const { return ItemBase::nbHChildren(); }

  //! i-ème enfant
  Cell hChild(Int32 i) const { return Cell(ItemBase::hChildBase(i)); }

  //! parent de niveau 0
  Cell topHParent() const { return Cell(ItemBase::topHParentBase()); }

  /*!
   * \returns \p true si l'item est actif (i.e. n'a pas de
   * descendants actifs), \p false  sinon. Notez qu'il suffit de vérifier
   * le premier enfant seulement. Renvoie toujours \p true si l'AMR est désactivé.
   */
  bool isActive() const { return ItemBase::isActive(); }

  bool isSubactive() const { return ItemBase::isSubactive(); }

  /*!
   * \returns \p true si l'item est un ancetre (i.e. a un
   * enfant actif ou un enfant ancetre), \p false sinon.
   * Renvoie toujours \p false si l'AMR est désactivé.
   */
  bool isAncestor() const { return ItemBase::isAncestor(); }

  /*!
   * \returns \p true si l'item a des enfants (actifs ou non),
   * \p false  sinon. Renvoie toujours \p false si l'AMR est désactivé.
   */
  bool hasHChildren() const { return ItemBase::hasHChildren(); }

  /*!
   * \returns le niveau de raffinement de l'item courant.  Si l'item
   * parent est \p NULL donc par convention il est au niveau 0,
   * sinon il est simplement au niveau superieur que celui de son parent.
   */
  Int32 level() const { return ItemBase::level(); }

  /*!
   * \returns le rang de l'enfant \p (iitem).
   * exemple: si rank = m_internal->whichChildAmI(iitem); donc
   * m_internal->hChild(rank) serait iitem;
   */
  Int32 whichChildAmI(const ItemInternal *iitem) const
  {
    return ItemBase::whichChildAmI(iitem->localId());
  }

  Cell* operator->() { return this; }
  const Cell* operator->() const { return this; }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Particule.
 * \ingroup Mesh
 */
class Particle
: public Item
{
 public:
  
  //! Type du localId()
  typedef ParticleLocalId LocalIdType;

  //! Constructeur d'une particule nulle
  Particle() : Item() {}

  //! Constructeur de recopie
  Particle(const Particle& rhs) : Item(rhs) {}

  //! Construit une référence à l'entité \a internal
  Particle(ItemInternal* ainternal) : Item(ainternal)
  { ARCANE_CHECK_KIND(isParticle); }

  //! Construit une référence à l'entité \a abase
  Particle(const ItemBase& abase) : Item(abase)
  { ARCANE_CHECK_KIND(isParticle); }

  //! Construit une référence à l'entité \a internal
  Particle(const ItemInternalPtr* internals,Int32 local_id)
  : Item(internals,local_id)
  { ARCANE_CHECK_KIND(isParticle); }

  //! Opérateur de copie
  Particle& operator=(ItemInternal* ainternal)
  {
    _set(ainternal);
    return (*this);
  }

  //! Opérateur de copie
  Particle& operator=(const Particle& from)
  {
    _set(from);
    return (*this);
  }

 public:

  //! Identifiant local de l'entité dans le sous-domaine du processeur
  ParticleLocalId itemLocalId() const { return ParticleLocalId{ m_local_id }; }

  /*!
   * \brief Maille à laquelle appartient la particule.
   * Il faut appeler setCell() avant d'appeler cette fonction.
   * \precondition hasCell() doit être vrai.
   */
  Cell cell() const { return Cell(ItemBase::cellBase(0)); }

  //! Maille connectée à la particule
  CellLocalId cellId() const { return CellLocalId(ItemBase::cellId(0)); }

  //! Vrai si la particule est dans une maille du maillage
  bool hasCell() const { return (ItemBase::cellId(0)!=NULL_ITEM_LOCAL_ID); }

  /*!
   * \brief Maille à laquelle appartient la particule ou maille nulle.
   * Retourne cell() si la particule est dans une maille ou la
   * maille nulle si la particule n'est dans aucune maille.
   */
  Cell cellOrNull() const
  {
    Int32 cell_local_id = ItemBase::cellId(0);
    if (cell_local_id==NULL_ITEM_LOCAL_ID)
      return Cell();
    return Cell(ItemBase::cellBase(0));
  }

  Particle* operator->() { return this; }
  const Particle* operator->() const { return this; }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 *
 * \brief classe degré de liberté.
 *
 * \ingroup Mesh
 *
 * Ce nouvel item DoF introduit une nouvelle gestion de la connectivité, déportée
 * dans des propriétés et non plus stockées dans l'ItemSharedInfo afin de pouvoir créer
 * de nouvelles connectivités en fonction des besoins de l'utilisateur. Par défaut aucune
 * connectivité n'est associée au DoF. Les connectivités nécessaires seront ajoutées par l'utilisateur.
 *
 */
class DoF
: public Item
{
 public:

  //! Constructeur d'une maille non connectée
  DoF() : Item() {}

  //! Constructeur de recopie
  DoF(const DoF& rhs) : Item(rhs) {}

  //! Construit une référence à l'entité \a internal
  DoF(ItemInternal* ainternal) : Item(ainternal)
  { ARCANE_CHECK_KIND(isDoF); }

  //! Construit une référence à l'entité \a abase
  DoF(const ItemBase& abase) : Item(abase)
  { ARCANE_CHECK_KIND(isDoF); }

  //! Construit une référence à l'entité \a internal
  DoF(const ItemInternalPtr* internals,Int32 local_id)
  : Item(internals,local_id)
  { ARCANE_CHECK_KIND(isDoF); }

  //! Opérateur de copie
  DoF& operator=(ItemInternal* ainternal)
  {
    _set(ainternal);
    return (*this);
  }

  //! Opérateur de copie
  DoF& operator=(const DoF& from)
  {
    _set(from);
    return (*this);
  }

  DoF* operator->() { return this; }
  const DoF* operator->() const { return this; }

  //! Identifiant local de l'entité dans le sous-domaine du processeur
  DoFLocalId itemLocalId() const { return DoFLocalId{ m_local_id }; }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline Edge Node::
edge(Int32 i) const
{
  return Edge(ItemBase::edgeBase(i));
}

inline Face Node::
face(Int32 i) const
{
  return Face(ItemBase::faceBase(i));
}

inline Cell Node::
cell(Int32 i) const
{
  return Cell(ItemBase::cellBase(i));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline Face Edge::
face(Int32 i) const
{
  return Face(ItemBase::faceBase(i));
}

inline Cell Edge::
cell(Int32 i) const
{
  return Cell(ItemBase::cellBase(i));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline Cell Face::
boundaryCell() const
{
  return Cell(ItemBase::boundaryCell());
}

inline Cell Face::
backCell() const
{
  return Cell(ItemBase::backCell());
}

inline Cell Face::
frontCell() const
{
  return Cell(ItemBase::frontCell());
}

inline Cell Face::
oppositeCell(Cell cell) const
{
  ARCANE_ASSERT((backCell()==cell || frontCell()==cell),("cell is not connected to the face"));
  return (backCell()==cell) ? frontCell() : backCell();
}

inline Cell Face::
cell(Int32 i) const
{
  return Cell(ItemBase::cellBase(i));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline ItemWithNodes Item::
toItemWithNodes() const
{
  ARCANE_CHECK_KIND(isItemWithNodes);
  return ItemWithNodes(*this);
}

inline Node Item::
toNode() const
{
  ARCANE_CHECK_KIND(isNode);
  return Node(*this);
}

inline Edge Item::
toEdge() const
{
  ARCANE_CHECK_KIND(isEdge);
  return Edge(*this);
}

inline Face Item::
toFace() const
{
  ARCANE_CHECK_KIND(isFace);
  return Face(*this);
}

inline Cell Item::
toCell() const
{
  ARCANE_CHECK_KIND(isCell);
  return Cell(*this);
}

inline Particle Item::
toParticle() const
{
  ARCANE_CHECK_KIND(isParticle);
  return Particle(*this);
}

inline DoF Item::
toDoF() const
{
  ARCANE_CHECK_KIND(isDoF);
  return DoF(*this);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline ItemLocalId::
ItemLocalId(Item item)
: m_local_id(item.localId())
{
}

template<typename ItemType> inline ItemLocalIdT<ItemType>::
ItemLocalIdT(ItemType item)
: ItemLocalId(item.localId())
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline Item ItemInfoListView::
operator[](ItemLocalId local_id) const
{
  return Item(ItemBase(ItemBaseBuildInfo(local_id.localId(), m_item_shared_info)));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline Item ItemInfoListView::
operator[](Int32 local_id) const
{
  return Item(ItemBase(ItemBaseBuildInfo(local_id, m_item_shared_info)));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename ItemType> inline ItemType ItemInfoListViewT<ItemType>::
operator[](ItemLocalId local_id) const
{
  return ItemType(ItemBase(ItemBaseBuildInfo(local_id.localId(), m_item_shared_info)));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename ItemType> inline ItemType ItemInfoListViewT<ItemType>::
operator[](Int32 local_id) const
{
  return ItemType(ItemBase(ItemBaseBuildInfo(local_id, m_item_shared_info)));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
