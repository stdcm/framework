#pragma once

#include <ALIEN/hypre/backend.h>
#include <ALIEN/hypre/options.h>
#include <ALIEN/Expression/Solver/SolverStats/SolverStater.h>
#include <ALIEN/Utils/Trace/ObjectWithTrace.h>

namespace Alien::Hypre {

    class Matrix;

    class Vector;

    class InternalLinearSolver
            : public IInternalLinearSolver<Matrix, Vector>, public ObjectWithTrace {
  public:

    typedef SolverStatus Status;

    InternalLinearSolver();

    InternalLinearSolver(const Options& options);

    virtual ~InternalLinearSolver() {}

  public:

    // Nothing to do
    void updateParallelMng(Arccore::MessagePassing::IMessagePassingMng *pm) {}

    bool solve(const Matrix &A, const Vector &b, Vector &x);

    bool hasParallelSupport() const { return true; }

    //! Etat du solveur
    const Status &getStatus() const;

    const SolverStat &getSolverStat() const { return m_stat; }

    std::shared_ptr<ILinearAlgebra> algebra() const;

  private:

    Status m_status;

    Arccore::Real m_init_time;
    Arccore::Real m_total_solve_time;
    Arccore::Integer m_solve_num;
    Arccore::Integer m_total_iter_num;

    SolverStat m_stat;
    Options m_options;

  private:

    void checkError(const Arccore::String &msg, int ierr, int skipError = 0) const;
  };

}