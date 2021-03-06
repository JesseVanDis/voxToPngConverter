
#ifndef VOXTOPNG_NODE_GROUP_HPP
#define VOXTOPNG_NODE_GROUP_HPP

#include <vector>
#include "node.hpp"

class NodeGroup : public Node
{
	public:
		explicit NodeGroup(const Context& context);
		error 				load(Loader& loader) override;
		const NodeGroup*	toNodeGroup() const override { return this; };
		void 				makeDirty() override;

	protected:

	private:
		vector<shared_ptr<NodeChild> > m_children;
};


#endif //VOXTOPNG_NODE_GROUP_HPP
