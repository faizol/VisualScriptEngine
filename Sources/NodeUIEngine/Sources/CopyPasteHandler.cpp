#include "CopyPasteHandler.hpp"
#include "Debug.hpp"

namespace NUIE
{

CopyPasteHandler::CopyPasteHandler () :
	tempNodeManager ()
{
}

bool CopyPasteHandler::CanPaste () const
{
	return !tempNodeManager.IsEmpty ();
}

bool CopyPasteHandler::CopyFrom (const NE::NodeManager& source, const NodeCollection& nodeCollection)
{
	class CopyFilter : public NE::NodeFilter
	{
	public:
		CopyFilter (const NodeCollection& nodeCollection) :
			nodeCollection (nodeCollection)
		{
			
		}

		virtual bool NeedToProcessNode (const NE::NodeId& nodeId) const override
		{
			return nodeCollection.Contains (nodeId);
		}

	private:
		const NodeCollection& nodeCollection;
	};

	tempNodeManager.Clear ();
	CopyFilter copyFilter (nodeCollection);
	return source.MergeTo (tempNodeManager, copyFilter);
}

bool CopyPasteHandler::PasteTo (NE::NodeManager& target)
{
	NE::AllNodesFilter allNodesFilter;
	return tempNodeManager.MergeTo (target, allNodesFilter);
}

}
