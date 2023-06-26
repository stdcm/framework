/* Author : havep at Mon Jun 30 16:04:32 2008
 * Generated by createNew
 */

#include <alien/kernels/petsc/linear_solver/super_lu/PETScSolverConfigSuperLUService.h>
#include <ALIEN/axl/PETScSolverConfigSuperLU_StrongOptions.h>

#include <arccore/message_passing/IMessagePassingMng.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
namespace Alien {
/** Constructeur de la classe */
#ifdef ALIEN_USE_ARCANE
PETScSolverConfigSuperLUService::PETScSolverConfigSuperLUService(
    const Arcane::ServiceBuildInfo& sbi)
: ArcanePETScSolverConfigSuperLUObject(sbi)
, PETScConfig(sbi.subDomain()->parallelMng()->isParallel())
{
  ;
}
#endif
PETScSolverConfigSuperLUService::PETScSolverConfigSuperLUService(
    Arccore::MessagePassing::IMessagePassingMng* parallel_mng,
    std::shared_ptr<IOptionsPETScSolverConfigSuperLU> options)
: ArcanePETScSolverConfigSuperLUObject(options)
, PETScConfig(parallel_mng->commSize() > 1)
{
}

//! Initialisation
void
PETScSolverConfigSuperLUService::configure(
    KSP& ksp, const ISpace& space, const MatrixDistribution& distribution)
{
  alien_debug([&] { cout() << "configure PETSc superlu solver"; });

  checkError(
      "Set solver tolerances", KSPSetTolerances(ksp, 1e-9, 1e-15, PETSC_DEFAULT, 2));

  checkError("Solver set type", KSPSetType(ksp, KSPPREONLY));
  PC pc;
  checkError("Get preconditioner", KSPGetPC(ksp, &pc));
  checkError("Preconditioner set type", PCSetType(pc, PCLU));

  if (isParallel())
#ifdef WIN32
    alien_fatal([&] { cout() << "SuperLUDist is not available for windows"; });
#else
    checkError("Set superlu_dist solver package",
        PCFactorSetMatSolverType(pc, MATSOLVERSUPERLU_DIST));
#endif
  else
    checkError(
        "Set superlu solver package", PCFactorSetMatSolverType(pc, MATSOLVERSUPERLU));

  KSPSetUp(ksp);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_PETSCSOLVERCONFIGSUPERLU(
    SuperLU, PETScSolverConfigSuperLUService);
ARCANE_REGISTER_SERVICE_PETSCSOLVERCONFIGSUPERLU(LU, PETScSolverConfigSuperLUService);

} // namespace Alien

REGISTER_STRONG_OPTIONS_PETSCSOLVERCONFIGSUPERLU();
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/