//
// Created by scion on 2/28/2024.
//

#include <PUGIXML/pugixml.hpp>

class Walker : public pugi::xml_tree_walker
{
 public:
	bool for_each(pugi::xml_node& node) override
	{
		return true;
	}
};

int main()
{
	pugi::xml_document file;
	file.load_file("test.xml");

	pugi::xml_node root = file.root();

	root.find_child()

	return 0;
}

