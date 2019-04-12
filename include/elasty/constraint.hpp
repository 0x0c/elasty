#ifndef constraint_hpp
#define constraint_hpp

#include <vector>
#include <Eigen/Core>

namespace elasty
{
    class Engine;

    class Constraint
    {
    public:

        Constraint(const Engine* engine, const std::vector<unsigned int>& indices) :
        m_engine(engine),
        m_indices(indices)
        {
        }

        virtual double calculateValue() = 0;
        virtual Eigen::VectorXd calculateGrad() = 0;

        const Engine* m_engine;
        const std::vector<unsigned int> m_indices;
    };

    class EnvironmentalCollisionConstraint final : public Constraint
    {
    public:

        EnvironmentalCollisionConstraint(const Engine* engine, const std::vector<unsigned int>& indices, const Eigen::Vector3d& n, const double d) :
        Constraint(engine, indices),
        m_n(n),
        m_d(d)
        {
            assert(m_indices.size() == 1);
        }

        double calculateValue() override;

        Eigen::VectorXd calculateGrad() override
        {
            return m_n;
        }

    private:

        const Eigen::Vector3d m_n;
        const double m_d;
    };
}

#endif /* constraint_hpp */