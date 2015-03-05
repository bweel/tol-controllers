#ifndef __NETWORKCTNODE_H__
#define __NETWORKCTNODE_H__
#include "NEAT_STL.h"
#include "NEAT_NetworkLink.h"
#include "NEAT_GeneticNodeGene.h"
#include "NEAT_NetworkNode.h"
#include "tinyxmlplus.h"
namespace NEAT
{
    static const string network_ct_node_type("NetworkCTNode");

    class NetworkCTNode : public NetworkNode
    {
    public:
    protected:
        double tau;

    public:
        NEAT_DLL_EXPORT NetworkCTNode(
            string _name,
            double _tau,
            bool _update=true
        );

        NEAT_DLL_EXPORT NetworkCTNode()
        {}

        NEAT_DLL_EXPORT virtual ~NetworkCTNode();

        virtual inline const string &getType()
        {
            return network_ct_node_type;
        }

        NEAT_DLL_EXPORT void addFromLink(NetworkLink *link);

        //This function computes the new value based on the children nodes.
        NEAT_DLL_EXPORT virtual void computeNewValue();
    protected:
    };
}
#endif
