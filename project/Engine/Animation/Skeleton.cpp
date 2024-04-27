#include "Skeleton.h"

namespace  SkeletonPace{

	


	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
	{
		Joint joint;
		joint.name = node.name;
		joint.locaalMatrix = node.localMatrix;
		joint.skeletonSpaceMatrix = Math::MakeIdentity4x4();
		joint.transform = node.transform;
		joint.index = int32_t(joints.size());//現在登録されている数をIndexに
		joint.parent = parent;
		joints.push_back(joint);//SkeletonのJoint列に追加
		for (const Node& child : node.children) {
			//子Jointを作成、そのIndexを登録
			int32_t childIndex = CreateJoint(child, joint.index, joints);
			joints[joint.index].children.push_back(childIndex);
		}
		return joint.index;
	}


	Skeleton CreateSkeleton(const Node& rootNode) {
		Skeleton skeleton;
		skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

		//名前とIndexのマッピングを行いアクセスしやすくする
		for (const Joint& joint : skeleton.joints) {
			skeleton.jointMap.emplace(joint.name, joint.index);
		}
		return skeleton;
	}


	

}