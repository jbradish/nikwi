/*
 * BadCFG
 * Copyright (C) 2005-2012 Kostas Michalopoulos
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Kostas Michalopoulos <badsector@runtimelegend.com>
 */

#ifndef __BADCFG_H_INCLUDED__
#define __BADCFG_H_INCLUDED__

/*! @overview
 * This is the documentation of the {@name BadCFG} library, made by Kostas
 * "{@emph Bad Sector}" Michalopoulos. {@name BadCFG} provides functionality
 * for parsing tree-structured text files, which (usually) can be used for
 * configuration purposes (so the {@emph cfg} in the name). The link below
 * ({@emph libbadcfg core}) will show the API reference of the library. You can
 * use this, or the frames at the left to navigate through the documentation.
 *
 * If you are looking for an introduction to using {@name BadCFG}, check the
 * {@ref tutorial}.
 */

/*! @guide tutorial "Tutorial" @toplevelguide
 *
 * @section "Introduction"
 * {@name BadCFG} is a C library that provides functionality for parsing
 * tree-structured text files, which can be used for configuration files
 * (although there is no restriction - they can be used for almost anything can
 * be represented using text files in a tree-structured manner). These files
 * have a C-like syntax, with childs being inside curly braces. An example is
 * given below:
 * @source
 * node = "value1"
 * anotherNode
 * {
 *	child1 = "value2"
 *	child2 = "value3"
 *	child3
 *	{
 *		subchild1 = "value4"
 *		subchild2 = "value5"
 *	}
 * }
 * yetAnotherNode = "value6"
 * {
 *	yetAnotherChild1 = "value7"
 *	yetAnotherChild2 = "value8"
 *	{
 *		yetAnotherSubchild1 = "value9"
 *		yetAnotherSubchild2 = "value10"
 *		{
 *			subsubchild = "value11"
 *		}
 *	}
 * }
 * @text
 * There is no restriction on the length or the depth of a node. 
 *
 * @section "Loading files"
 * The {@link BadCFGNode} structure represents a single node in the tree. The
 * structure's {@link BadCFGNode.name} holds the node's name (such as
 * {@emph yetAnotherNode} in the above example) and the {@link BadCFGNode.value}
 * holds the node's value (such as {@emph value6} in the above example). All
 * nodes belong to a single node called {@emph the root node}. The
 * {@link loadBadCFGFile} function loads and parses a text file like the above
 * and returns a root node that contains all the nodes found in the file. The
 * root node's name is always {@name root}. Note that if a file has a structure
 * like this:
 * @source
 * config
 * {
 * 	key1 = "value1"
 *	key2 = "value2"
 *	key3
 *	{
 *		subkey1 = "value3"
 *		subkey2 = "value4"
 *	}
 *	key4 = "value5"
 * }
 * @text
 * then the root node will contain a single child (the {@emph config} node)
 * which in turn will contain the rest of the nodes. This may look strange to
 * people with some XML background, since XML requires from the author to
 * define the root node in the document. However, this isn't practical for
 * simple configuration files like this:
 * @source
 * myappconfig = "/home/badsector/.myapp"
 * showfullscreen = "yes"
 * @text
 * The above code will generate a root node with two child nodes,
 * {@emph myappconfig} and {@emph showfullscreen}. Notice how simple and
 * readable this syntax is, compared to what had to be used for an XML file:
 * @source
 * <?xml version="1.0"?>
 * <config>
 * <myappconfig>/home/badsector/.myapp</myappconfig>
 * <showfullscreen>yes</showfullscreen>
 * </config>
 * @text
 * However, this isn't a comparison between {@name BadCFG} and XML code -
 * both have their purpose; it's just that i believe that XML is
 * {@emph not} suitable for all purposes :-).
 *
 * @section "Parsing code from memory"
 * If you can't use the {@link loadBadCFGFile} function because the code doesn't
 * lie in the local filesystem, you need to use the {@link parseBadCFGCode}
 * function, which does the same job as {@name loadBadCFGFile}, but instead
 * of loading a file and returning the root node, it parses the given code and
 * returns the root node. In fact, {@name loadBadCFGFile} loads the given file
 * in memory and calls this function to return the root node. This is obvious
 * by the source code of the function (see the {@name main.c} file in the
 * {@name src} directory for the code).
 *
 * @section "Asking for a value"
 * Using the {@link getBadCFGValue} function, you can "ask" for the value of a
 * node. This is simply done like this:
 * @source
 * char *srcdir = getBadCFGValue(rootnode, "srcdir", NULL);
 * @text
 * The first argument is the node's parent. In simple trees, you only provide
 * the root node (but any node can be used there). The second argument is the
 * node's name and the third argument is the default value to be returned if
 * the node wasn't found (NULL in this case). Note that {@name BadCFG} allows
 * you to give a "path" for the node you want, with each node name being
 * separated with a period ('.'). So, for example, if you want to get the
 * value of {@name subchild2} from the first example, you can write:
 * @source
 * getBadCFGValue(rootnode, "anotherNode.child3.subchild2", NULL);
 * @text
 * Note that {@emph you must not free or alter the contents of the returned
 * string}! The string that {@name getBadCFGValue} returns is the
 * {@link BadCFGNode.value} member directly - it doesn't {@code strdup} it or
 * anything like that. The {@link destroyBadCFGNode} function will free it
 * when you use it to destroy the root node (see below for that).
 *
 * Another function you may want to check out, is {@link getBadCFGNode}. This
 * function has the same syntax as {@name getBadCFGValue} (without the default
 * value argument) and returns a {@link BadCFGNode} structure or NULL if the
 * node wasn't found. Like {@name getBadCFGValue}, this one can use paths too.
 *
 * @section "Scanning the nodes"
 * Another method you can use to get the values of the nodes in a tree, is to
 * use the {@link BadCFGNode.child} member in a {@code for} structure. For
 * example, to scan the childs of the {@name anotherNode} node from the first
 * example, use the following code:
 * @source
 * BadCFGNode	*anotherNode = getBadCFGNode(rootnode, "anotherNode");
 * BadCFGNode	*child;
 * for (child=anotherNode->child;child;child=child->next)
 *	printf("name=`%s' value=`%s'\n",
 *		child->name,
 *		child->value?child->value:"(no value)");
 * @text
 * Note the conditional operator when accessing the {@link BadCFGNode.value}
 * member; this member {@emph can} be NULL if no value was given (no '='
 * character was used in the code to give a value to the node). This is the
 * case with {@code child3} from the example.
 *
 * @section "Destroying the tree"
 * When you no longer need the tree, you must destroy it. This can be done by
 * using the {@link destroyBadCFGNode} function with the root node. Note that
 * you don't have to destroy each node manually; the {@name destroyBadCFGNode}
 * function will destroy the given node's children in a recursive manner, so by
 * using it with the root node, the whole tree is destroyed.
 */

/*! @module "libbadcfg core"
 *
 * Provides functionality for parsing tree-structured text files.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef
/* hack to make SlashDOC's C++ parser to see this as a class */
/**
 * BadCFGNode holds information about a single node in the generated tree.
 */
struct BadCFGNode
{
	/** The node's name. This is the name given at the 'left' side of the
	 * configuration line. */
	char			*name;
	/** The node's value. If the node has a value (an assignment given in
	 * the configuration file), this is the value. Else it is NULL. */
	char			*value;
	/** The parent of the node. */
	struct BadCFGNode	*parent;
	/** The node's first child. */
	struct BadCFGNode	*child;
	/** The node's last child. */
	struct BadCFGNode	*lastChild;
	/** The previous node. */
	struct BadCFGNode	*previous;
	/** The next node. */
	struct BadCFGNode	*next;
} BadCFGNode;

/**
 * Loads and parses a BadCFG file. The given file is loaded in memory and passed
 * to the {@link parseBadCFGCode} function in order to generate a tree of
 * {@link BadCFGNode} objects.
 *
 * @param fileName The filename to open.
 * @return The root node of the BadCFGNode tree.
 */
BadCFGNode *loadBadCFGFile(char *fileName);

/**
 * Parses the given BadCFG code and generates a tree of {@link BadCFGNode}
 * objects. Unless you want to use some other means of loading configuration
 * files (such as directly from an archive), you should use
 * {@link loadBadCFGFile} instead.
 *
 * @param code The BadCFG code as loaded from the file.
 * @return The root node of the BadCFGNode tree.
 */
BadCFGNode *parseBadCFGCode(char *code);

/**
 * Returns the {@link BadCFGNode} child of the given node that has the given
 * name. This must be a direct child (no searching is performed). The name can
 * consist of several names in a {@code parent.child} fashion. For example, if
 * the root node has a child named {@name info} and that child has a child
 * named {@name title}, then you can get the value of that last child by using
 * the name {@name info.title}.
 *
 * @param root The root (or parent) node to search for the name in.
 * @param name The name of the child or the path to the child as described
 *	above.
 */
BadCFGNode *getBadCFGNode(BadCFGNode *root, char *name);

/**
 * Returns the value of the given node or a default value if the node wasn't
 * found. The rules about the name are the same as described in the
 * {@link getBadCFGNode} function. In fact, this calls the
 * {@name getBadCFGNode} function and returns it's {@link BadCFGNode.value}
 * field or the value of {@name defaultValue} if the node wasn't found.
 *
 * @param root The root (or parent) node to search for the name in.
 * @param name The name of the child or the path to the child.
 * @param defaultValue The default value to be returned if the child wasn't
 *	found.
 */
char *getBadCFGValue(BadCFGNode *root, char *name, char *defaultValue);

/**
 * Destroys the given node (or tree). This must be used when the
 * {@link BadCFGNode} tree returned from {@link parseBadCFGCode} or
 * {@link loadBadCFGFile} is not needed anymore. You don't need to destroy each
 * node manually; this function will delete the given node, including it's
 * children and grandchildren.
 *
 * @param node The node to destroy.
 */
void destroyBadCFGNode(BadCFGNode *node);

#ifdef __cplusplus
};
#endif

#endif
