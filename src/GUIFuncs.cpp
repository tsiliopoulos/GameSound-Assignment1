#include "GUIFuncs.h"
#include "imgui\imgui.h"

// Takes in a root of the objects you want to draw
// Returns the pointer of the selected gameobject. If no object is selected, return nullptr
GameObject* drawHeadHierarchyUI(GameObject* root)
{
	static GameObject* selectedObject = nullptr;
	
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	// Draw root
	bool isRootExpanded = false;
	if (root->getParent() == nullptr)
	{
		isRootExpanded = ImGui::TreeNodeEx(root->name.c_str(), selectedObject == root ? node_flags | ImGuiTreeNodeFlags_Selected : node_flags);
		if (!isRootExpanded)
			return selectedObject;
	}

	if (ImGui::IsItemClicked())
		selectedObject = root;

	// Draw children
	auto currentChildrenList = root->getChildren();
	for (int i = 0; i < currentChildrenList.size(); i++)
	{
		bool node_open = ImGui::TreeNodeEx(currentChildrenList[i]->name.c_str(), selectedObject == currentChildrenList[i] ? node_flags | ImGuiTreeNodeFlags_Selected : node_flags);

		if (ImGui::IsItemClicked())
			selectedObject = currentChildrenList[i];

		if (node_open)
		{
			// If this node is expanded, draw the UI for the children nodes, if it has any
			if (currentChildrenList[i]->getChildren().size() > 0)
				drawHeadHierarchyUI(currentChildrenList[i]);

			ImGui::TreePop();
		}
	}

	if (isRootExpanded)
		ImGui::TreePop();

	return selectedObject;
}

// Takes in a root of the objects you want to draw
// Returns the pointer of the selected gameobject. If no object is selected, return nullptr
GameObject* drawSpeakerHierarchyUI(GameObject* root)
{
	static GameObject* selectedObject = nullptr;

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	// Draw root
	bool isRootExpanded = false;
	if (root->getParent() == nullptr)
	{
		isRootExpanded = ImGui::TreeNodeEx(root->name.c_str(), selectedObject == root ? node_flags | ImGuiTreeNodeFlags_Selected : node_flags);
		if (!isRootExpanded)
			return selectedObject;
	}

	if (ImGui::IsItemClicked())
		selectedObject = root;

	// Draw children
	auto currentChildrenList = root->getChildren();
	for (int i = 0; i < currentChildrenList.size(); i++)
	{
		bool node_open = ImGui::TreeNodeEx(currentChildrenList[i]->name.c_str(), selectedObject == currentChildrenList[i] ? node_flags | ImGuiTreeNodeFlags_Selected : node_flags);

		if (ImGui::IsItemClicked())
			selectedObject = currentChildrenList[i];

		if (node_open)
		{
			// If this node is expanded, draw the UI for the children nodes, if it has any
			if (currentChildrenList[i]->getChildren().size() > 0)
				drawHeadHierarchyUI(currentChildrenList[i]);

			ImGui::TreePop();
		}
	}

	if (isRootExpanded)
		ImGui::TreePop();

	return selectedObject;
}