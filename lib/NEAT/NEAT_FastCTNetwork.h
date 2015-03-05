#ifndef FastCTNetwork_H_INCLUDED
#define FastCTNetwork_H_INCLUDED

#include "NEAT_Network.h"

#include "NEAT_NetworkNode.h"
#include "NEAT_NetworkLink.h"
#include "NEAT_NetworkIndexedLink.h"

namespace NEAT
{

    /**
     *  The FastCTNetwork class is designed to be faster at the cost
     *  of being less dynamic.  Adding/Removing links and nodes
     *  is not supported with this network.
     */
    template<class Type>
    class FastCTNetwork : public Network<Type>
    {
        int numNodes;
        int numLinks;
        map<string,int> nodeNameToIndex;
        char *data;
        Type *nodeValues;
        Type *nodeNewValues;
        std::vector<double> nodeTauValues;
        ActivationFunction *activationFunctions;
        NetworkIndexedLink<Type> *links;
        map<pair<int,int>,int> nodeLinkMap;

        /**
         * numConstantNodes holds the index of the first node that is updated.  All nodes before
         * numConstantNodes are constant
         */
        int numConstantNodes;

    public:
        /**
         *  (Constructor) Create a Network with the inputed geneology
         */
        NEAT_DLL_EXPORT FastCTNetwork(
            const vector<GeneticNodeGene> &_nodes,
            const vector<GeneticLinkGene> &_links
        );

        /**
         *  (Constructor) Empty Constructor
         */
        NEAT_DLL_EXPORT FastCTNetwork();

        /**
         *  (Assignment) Copies the network
         */
        NEAT_DLL_EXPORT FastCTNetwork& operator=(const FastCTNetwork &other);

        /**
         *  (Copy Constructor) Copies the network
         */
        NEAT_DLL_EXPORT FastCTNetwork(const FastCTNetwork &other);

        NEAT_DLL_EXPORT virtual ~FastCTNetwork();

        //NetworkNode *getNode(const string name);

        /**
         *  hasNode: Returns "true" if the node with the specified name
         *  exists
         */
        NEAT_DLL_EXPORT virtual bool hasNode(const string &nodeName);

        /**
         *  getValue: gets the value for a specified node
         */
        NEAT_DLL_EXPORT Type getValue(const string &nodeName);

        /**
         *  setValue: sets the value for a specified node
         */
        NEAT_DLL_EXPORT void setValue(const string &nodeName,Type newValue);

        /**
         *  getLink: gets the link according to its index when created
         */
        NetworkIndexedLink<Type> *getLink(int index)
        {
            return &links[index];
        }

        /**
         *  getLink: gets the link according to the names
         *  of it's nodes
         */
        NEAT_DLL_EXPORT NetworkIndexedLink<Type> *getLink(const string &fromNodeName,const string &toNodeName);

        NEAT_DLL_EXPORT bool hasLink(const string &fromNodeName,const string &toNodeName);

        NEAT_DLL_EXPORT Type getLinkWeight(const string &fromNodeName,const string &toNodeName);

        /**
         *  getLinkCount: gets the number of links
         */
        inline const int getLinkCount()
        {
            return numLinks;
        }

        /**
         * reinitialize: This resets the state of the network
         * to its initial state
         */
        NEAT_DLL_EXPORT void reinitialize();

        /**
         * update: This updates the network.
         * If the network has not been updated since construction or
         * reinitialize(), the network will be activated.  This means
         * it will update (1+ExtraActivationUpdates+iterations) times!
         * Otherwise, it will update (iterations) times.  If you do not
         * want the extra updates, call dummyActivation() before the first
         * update.
         */
        NEAT_DLL_EXPORT void updateFixedIterations(int iterations);

        NEAT_DLL_EXPORT virtual void update()
        {
            updateFixedIterations(1);
        }

        NEAT_DLL_EXPORT void print();

        NEAT_DLL_EXPORT void clearAllLinkWeights();

    protected:
        void copyFrom(const FastCTNetwork &other);

        Type runActivationFunction(Type value,ActivationFunction function,bool signedActivation,bool usingTanhSigmoid);
    };

}


#endif // FastCTNetwork_H_INCLUDED
