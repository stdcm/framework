// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* SimpleCsvComparatorService.cc                                   (C) 2000-2022 */
/*                                                                           */
/* Service permettant de construire et de sortir un tableau au formet csv.   */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/std/SimpleCsvReaderWriter.h"

#include <arcane/Directory.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool SimpleCsvReaderWriter::
writeCsv(Directory dst, String file)
{
  if(!createDirectory(dst)) {
    return false;
  }

  std::ofstream ofile(dst.file(file).localstr());
  if (ofile.fail())
    return false;

  _print(ofile);

  ofile.close();
  return true;
}

bool SimpleCsvReaderWriter::
readCsv(Directory src, String file)
{
  clearCsv();

  std::ifstream stream;

  // Pas de fichier, pas de chocolats.
  if(!_openFile(stream, src, file)) {
    return false;
  }

  std::string line;

  // S'il n'y a pas de première ligne, on arrete là.
  // Un fichier écrit par SimpleCsvOutput possède toujours au
  // moins une ligne.
  if(!std::getline(stream, line)) {
    _closeFile(stream);
    return false;
  }

  // Sinon, on a la ligne d'entête, contenant les noms
  // des colonnes (et le nom du tableau).
  String ligne(line);

  {
    StringUniqueArray tmp;
    ligne.split(tmp, m_separator);
    // Normalement, tmp[0] existe toujours (peut-être = à "" (vide)).
    m_name_tab = tmp[0];
    m_name_columns = tmp.subConstView(1, tmp.size());
  }

  // S'il n'y a pas d'autres lignes, c'est qu'il n'y a que des 
  // colonnes vides (ou aucunes colonnes) et aucunes lignes.
  if(!std::getline(stream, line)) {
    _closeFile(stream);
    return true;
  }

  // Maintenant que l'on a le nombre de colonnes, on peut définir
  // la dimension 2 du tableau de valeurs.
  m_values_csv.resize(1, m_name_columns.size());

  Integer compt_line = 0;

  do{
    // On n'a pas le nombre de lignes en avance,
    // donc on doit resize à chaque tour.
    m_values_csv.resize(compt_line+1);

    // On split la ligne récupéré.
    StringUniqueArray splitted_line;
    String ligne(line);
    ligne.split(splitted_line, m_separator);

    // Le premier élement est le nom de ligne.
    m_name_rows.add(splitted_line[0]);

    // Les autres élements sont des Reals.
    for(Integer i = 1; i < splitted_line.size(); i++){
      m_values_csv[compt_line][i-1] = std::stod(splitted_line[i].localstr());
    }

    compt_line++;
  } while(std::getline(stream, line));

  _closeFile(stream);
  return true;
}

bool SimpleCsvReaderWriter::
clearCsv()
{
  m_values_csv.clear();

  m_name_rows.clear();
  m_name_columns.clear();
  return true;
}

void SimpleCsvReaderWriter::
printCsv(Integer only_proc)
{
  if (only_proc != -1 && m_mesh->parallelMng()->commRank() != only_proc)
    return;
  _print(std::cout);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


bool SimpleCsvReaderWriter::
createDirectory(Directory& dir)
{
  int sf = 0;
  if (m_mesh->parallelMng()->commRank() == 0) {
    sf = dir.createDirectory();
  }
  if (m_mesh->parallelMng()->commSize() > 1) {
    sf = m_mesh->parallelMng()->reduce(Parallel::ReduceMax, sf);
  }
  return sf == 0;
}

bool SimpleCsvReaderWriter::
isFileExist(Directory dir, String file)
{
  std::ifstream stream;
  bool fin = _openFile(stream, dir, file);
  _closeFile(stream);
  return fin;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool SimpleCsvReaderWriter::
_openFile(std::ifstream& stream, Directory dir, String file)
{
  stream.open(dir.file(file).localstr(), std::ifstream::in);
  return stream.good();
}

void SimpleCsvReaderWriter::
_closeFile(std::ifstream& stream)
{
  stream.close();
}

void SimpleCsvReaderWriter::
_print(std::ostream& stream)
{
  // On enregistre les infos du stream pour les restaurer à la fin.
  std::ios_base::fmtflags save_flags = stream.flags();
  std::streamsize save_prec = stream.precision();

  if (m_is_fixed_print) {
    stream << std::setiosflags(std::ios::fixed);
  }
  stream << std::setprecision(m_precision_print);

  stream << m_name_tab << m_separator;

  for (Integer j = 0; j < m_name_columns.size(); j++) {
    stream << m_name_columns[j] << m_separator;
  }
  stream << std::endl;

  for (Integer i = 0; i < m_values_csv.dim1Size(); i++) {
    stream << m_name_rows[i] << m_separator;
    ConstArrayView<Real> view = m_values_csv[i];
    for (Integer j = 0; j < m_values_csv.dim2Size(); j++) {
      stream << view[j] << m_separator;
    }
    stream << std::endl;
  }

  stream.flags(save_flags);
  stream.precision(save_prec);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
