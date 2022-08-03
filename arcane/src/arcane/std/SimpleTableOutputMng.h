﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* TODO                                   (C) 2000-2022 */
/*                                                                           */
/*    */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifndef ARCANE_STD_SIMPLETABLEOUTPUTMNG_H
#define ARCANE_STD_SIMPLETABLEOUTPUTMNG_H

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/ISimpleTableMng.h"
#include "arcane/ISimpleTableReaderWriter.h"
#include "arcane/ISimpleTableOutputMng.h"
#include "arcane/Directory.h"
#include "arcane/utils/Array.h"
#include "arcane/utils/Array2.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class SimpleTableOutputMng
: public ISimpleTableOutputMng
{
 public:
  SimpleTableOutputMng(ISimpleTableReaderWriter* strw)
  : m_sti(strw->internal())
  , m_strw(strw)
  {

  }

  // SimpleTableOutputMng()
  // : m_sti(nullptr)
  // , m_strw(nullptr)
  // {
  //   std::cout << "Attention, STOM vide !" << std::endl;
  // }

  virtual ~SimpleTableOutputMng() = default;

 public:
  bool init() override;
  bool init(String name_table) override;
  bool init(String name_table, String name_dir) override;

  void print(Integer only_proc) override;
  bool writeFile(Integer only_proc) override;
  bool writeFile(Directory root_dir, Integer only_proc) override;
  bool writeFile(String dir, Integer only_proc) override;

  Integer precision() override;
  void setPrecision(Integer precision) override;

  bool fixed() override;
  void setFixed(bool fixed) override;

  String outputDir() override;
  void setOutputDir(String dir) override;

  String tabName() override;
  void setTabName(String name) override;
  String fileName() override;
  
  Directory outputPath() override;
  Directory rootPath() override;
  
  String outputFileType() override;

  bool isOneFileByProcsPermited() override;

  SimpleTableInternal* internal() override;
  void setInternal(SimpleTableInternal* sti) override;
  void setInternal(SimpleTableInternal& sti) override;

  ISimpleTableReaderWriter* readerWriter() override;
  void setReaderWriter(ISimpleTableReaderWriter* strw) override;
  void setReaderWriter(ISimpleTableReaderWriter& strw) override;

 private:
  String _computeFinal();
  void _computeName();
  bool _createDirectory(Directory dir);
  bool _createOutputDirectory();
  bool _createRoot();

 private:
  String m_name_output_dir;

  Directory m_root;

  bool m_name_tab_computed;
  bool m_name_tab_only_once;

  SimpleTableInternal* m_sti;
  ISimpleTableReaderWriter* m_strw;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arcane

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
