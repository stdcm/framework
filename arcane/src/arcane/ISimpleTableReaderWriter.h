﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* ISimpleTableReaderWriter.h                                  (C) 2000-2022 */
/*                                                                           */
/* Interface représentant un lecteur/écrivain de tableau simple.             */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifndef ARCANE_ISIMPLETABLEREADERWRITER_H
#define ARCANE_ISIMPLETABLEREADERWRITER_H

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/ISimpleTableInternalMng.h"

#include "arcane/Directory.h"
#include "arcane/IParallelMng.h"
#include "arcane/ItemTypes.h"
#include "arcane/utils/Iostream.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/**
 * @brief Classe contenant deux méthodes statiques
 * utile pour les implémentations.
 * 
 */
class ARCANE_CORE_EXPORT SimpleTableReaderWriterUtils
{
 public:
  static bool createDirectoryOnlyProcess0(IParallelMng* parallel_mng, const Directory& directory)
  {
    int sf = 0;
    if (parallel_mng->commRank() == 0) {
      sf = directory.createDirectory();
    }
    if (parallel_mng->commSize() > 1) {
      sf = parallel_mng->reduce(Parallel::ReduceMax, sf);
    }
    return sf == 0;
  };
  static bool isFileExist(const Directory& directory, const String& file)
  {
    std::ifstream stream;
    stream.open(directory.file(file).localstr(), std::ifstream::in);
    bool fin = stream.good();
    stream.close();
    return fin;
  };
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/**
 * @brief Interface de classe permettant de lire un fichier et d'écrire
 * un fichier avec ou à partir d'un SimpleTableInternal.
 * 
 * Le fichier lu devra, de préférence, avoir été écrit par une implémentation 
 * de cette même interface.
 * 
 * Impérativement donc, un fichier écrit par une implémentation de cette
 * interface devra pouvoir être lu par cette même implémentation.
 * 
 * L'implémentation ne devra pas détruire l'objet SimpleTableInternal
 * pointé par le pointeur utilisé. C'est à l'appelant de gérer ça.
 */
class ARCANE_CORE_EXPORT ISimpleTableReaderWriter
{
 public:
  virtual ~ISimpleTableReaderWriter() = default;

 public:
  /**
   * @brief Méthode permettant d'écrire un tableau simple dans un fichier.
   * 
   * L'extension sera ajouté par l'implémentation.
   * 
   * Le répertoire de destination sera créé par l'implémentation s'il
   * n'existe pas.
   * 
   * Les élements de SimpleTableInternal qui doivent impérativement
   * être écrits sont :
   * - les noms des lignes    (m_row_names),
   * - les noms des colonnes  (m_column_names),
   * - le nom du tableau      (m_table_name),
   * - les valeurs du tableau (m_values).
   * 
   * Les autres élements de SimpleTableInternal ne sont pas obligatoire.
   * 
   * @param dst Le répertoire de destination.
   * @param file_name Le nom du fichier (sans extension).
   * @return true Si le fichier a bien été écrit.
   * @return false Si le fichier n'a pas pu être écrit.
   */
  virtual bool writeTable(const Directory& dst, const String& file_name) = 0;

  /**
   * @brief Méthode permettant de lire un fichier contenant un tableau simple.
   * 
   * L'extension sera ajouté par l'implémentation.
   * 
   * Un appel à SimpleTableInternal::clear() devra être effectué avant la lecture.
   * 
   * Les élements qui doivent impérativement être récupérés sont :
   * - les noms des lignes    (m_row_names),
   * - les noms des colonnes  (m_column_names),
   * - le nom du tableau      (m_table_name),
   * - les valeurs du tableau (m_values).
   * 
   * Les élements qui doivent être déduit si non récupérés sont :
   * - les tailles des lignes   (m_row_sizes),
   * - les tailles des colonnes (m_column_sizes).
   * 
   * Déduction par défaut pour m_row_sizes :
   * - len(m_row_sizes) = len(m_row_names)
   * - m_row_sizes[*]   = m_values.dim2Size()
   * 
   * Déduction par défaut pour m_column_sizes :
   * - len(m_column_sizes) = len(m_column_names)
   * - m_column_sizes[*]   = m_values.dim1Size()
   * 
   * 
   * @param src Le répertoire source.
   * @param file_name Le nom du fichier (sans extension).
   * @return true Si le fichier a bien été lu.
   * @return false Si le fichier n'a pas pu être lu.
   */
  virtual bool readTable(const Directory& src, const String& file_name) = 0;

  /**
   * @brief Méthode permettant d'effacer le contenu de l'objet
   * SimpleTableInternal.
   */
  virtual void clearInternal() = 0;

  /**
   * @brief Méthode permettant d'écrire le tableau dans la
   * sortie standard.
   * 
   * Le format d'écriture est libre (pour l'implémentation
   * csv, l'écriture se fait pareil que dans un fichier csv).
   */
  virtual void print() = 0;

  /**
   * @brief Méthode permettant de récupérer la précision actuellement
   * utilisée pour l'écriture des valeurs.
   * 
   * @return Integer La précision.
   */
  virtual Integer precision() = 0;

  /**
   * @brief Méthode permettant de modifier la précision du print.
   * 
   * Aussi bien pour la méthode 'print()' que la méthode 'writetable()'.
   * 
   * @note Un appel à cette méthode sans le paramètre définira la précision
   * par défaut.
   * 
   * @param precision La nouvelle précision.
   */
  virtual void setPrecision(Integer precision = 6) = 0;

  /**
   * @brief Méthode permettant de savoir si le frag 'std::fixed' est
   * actif ou non pour l'écriture des valeurs.
   * 
   * @return true Si oui.
   * @return false Si non.
   */
  virtual bool isFixed() = 0;
  /**
   * @brief Méthode permettant de définir le flag 'std::fixed' ou non.
   * 
   * Aussi bien pour la méthode 'print()' que la méthode 'writetable()'.
   * 
   * Ce flag permet de 'forcer' le nombre de chiffre après la virgule à
   * la précision voulu. Par exemple, si l'on a appelé 'setPrecision(4)',
   * et que l'on appelle 'setFixed(true)', le print de '6.1' donnera '6.1000'.
   * 
   * @note Un appel à cette méthode sans le paramètre définira le flag
   * par défaut.
   * 
   * @param fixed Si le flag 'std::fixed' doit être défini ou non.
   */
  virtual void setFixed(bool fixed = true) = 0;

  /**
   * @brief Méthode permettant de récupérer le type de fichier
   * qui sera écrit par l'implémentation. ("csv" sera retourné
   * pour l'implémentation csv).
   * 
   * @return String Le type/l'extension du fichier utilisé.
   */
  virtual String fileType() = 0;

  /**
   * @brief Méthode permettant de récupérer le pointeur vers l'objet
   * SimpleTableInternal utilisé.
   * 
   * @return SimpleTableInternal* Le pointeur utilisé. 
   */
  virtual SimpleTableInternal* internal() = 0;

  /**
   * @brief Méthode permettant de définir un pointeur vers
   * SimpleTableInternal.
   * 
   * @warning Il est déconseillé d'utiliser cette méthode, sauf si
   * vous savez ce que vous faite. La destruction de l'objet reste
   * à la charge de l'appelant.
   * 
   * @param simple_table_reader_writer Le pointeur vers SimpleTableInternal.
   */
  virtual void setInternal(SimpleTableInternal* simple_table_internal) = 0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
