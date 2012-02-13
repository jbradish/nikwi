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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <badcfg.h>

static char *strClone(char *string)
{
	char	*newStr = (char*)malloc(strlen(string) + 1);
	strcpy(newStr, string);
	return newStr;
}

static void skipWhitespace(char *code, unsigned int *head)
{
	while (code[*head] && (isspace(code[*head]) || code[*head] == '#'))
	{
		if (code[*head] == '#')
		{
			while (code[*head] && code[*head] != '\n')
				(*head)++;
		}
		else
			(*head)++;
	}
}

static char *getToken(char *code, unsigned int *head, char *stop)
{
	static char	token[16384];
	int		len = 0;
	char		sch;
	skipWhitespace(code, head);
	
	if (code[*head] == '"' || code[*head] == '\'')
	{
		sch = code[*head];
		(*head)++;
		while (code[*head] && code[*head] != sch)
		{
			if (code[*head] == '\\')
			{
				(*head)++;
				switch (code[*head])
				{
					case 'n': token[len++] = '\n'; break;
					case 't': token[len++] = '\t'; break;
					case 'r': token[len++] = '\r'; break;
					case 'e': token[len++] = 27; break;
					case 'b': token[len++] = 26; break;
					case '\\': token[len++] = '\\'; break;
					case '\'': token[len++] = '\''; break;
					case '"': token[len++] = '"'; break;
					default:
						token[len++] = '\\';
						token[len++] = code[*head];
				}
				(*head)++;
			}
			else
			{
				if (len < 16383)
					token[len++] = code[(*head)++];
			}
		}
		if (code[*head] == sch)
			(*head)++;
		token[len] = 0;
		skipWhitespace(code, head);
		return token;
	}
	
	while (code[*head] && !isspace(code[*head]) &&
		!strchr(stop, code[*head]))
		token[len++] = code[(*head)++];
		
	skipWhitespace(code, head);
	token[len] = 0;
	return token;
}

static void parseNode(BadCFGNode *parent, char *code, unsigned int *head)
{
	while (code[*head])
	{
		BadCFGNode	*node;
		char		*token;
		skipWhitespace(code, head);
		if (!code[*head])
			return;
		if (code[*head] == '}')
			return;
		token = getToken(code, head, "={}");
		if (!token[0])
			return;
		node = (BadCFGNode*)malloc(sizeof(BadCFGNode));
		node->name = strClone(token);
		node->value = NULL;
		node->parent = parent;
		node->child = NULL;
		node->lastChild = NULL;
		node->previous = parent->lastChild;
		node->next = NULL;
		if (parent->lastChild)
			parent->lastChild->next = node;
		else
			parent->child = node;
		parent->lastChild = node;
		
		if (code[*head] == '=')
		{
			(*head)++;
			token = getToken(code, head, "={}");
			node->value = strClone(token);
		}
		
		if (code[*head] == '{')
		{
			(*head)++;
			parseNode(node, code, head);
			if (code[*head] == '}')
				(*head)++;
		}
	}
}

BadCFGNode *loadBadCFGFile(char *fileName)
{
	BadCFGNode	*node;
	FILE		*f = fopen(fileName, "rb");
	char		*code;
	unsigned int	size;
	
	if (!f)
		return NULL;
	
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	if (!size)
	{
		fclose(f);
		return NULL;
	}
	
	code = (char*)malloc(size + 1);
	fread(code, 1, size, f);
	fclose(f);
	code[size] = 0;
	
	node = parseBadCFGCode(code);
	
	free(code);
	
	return node;
}

BadCFGNode *parseBadCFGCode(char *code)
{
	unsigned int	head = 0;
	
	BadCFGNode	*node = (BadCFGNode*)malloc(sizeof(BadCFGNode));
	memset(node, 0, sizeof(BadCFGNode));
	node->name = strClone("root");
	parseNode(node, code, &head);
	
	return node;
}

static BadCFGNode *getChild(BadCFGNode *node, char *name)
{
	BadCFGNode	*child;
	if (!node)
		return NULL;
	child = node->child;
	for (;child;child = child->next)
		if (!strcmp(name, child->name))
			return child;
	return NULL;
}

BadCFGNode *getBadCFGNode(BadCFGNode *root, char *name)
{
	unsigned int	head = 0;
	char		*token = getToken(name, &head, ".");
	BadCFGNode	*node = getChild(root, token);
	if (!node)
		return NULL;
	while (name[head] == '.')
	{
		head++;
		token = getToken(name, &head, ".");
		node = getChild(node, token);
		if (!node)
			return NULL;
	}
	return node;
}

char *getBadCFGValue(BadCFGNode *root, char *name, char *defaultValue)
{
	BadCFGNode	*node = getBadCFGNode(root, name);
	if (!node || !node->value)
		return defaultValue;
	return node->value;
}

void destroyBadCFGNode(BadCFGNode *node)
{
	BadCFGNode	*next, *child;
	
	child = node->child;
	while (child)
	{
		next = child->next;
		destroyBadCFGNode(child);
		child = next;
	}
	
	if (node->parent)
	{
		if (node->previous)
			node->previous->next = node->next;
		else
			node->parent->child = node->next;
		if (node->next)
			node->next->previous = node->previous;
		else
			node->parent->lastChild = node->previous;
	}
	
	free(node->name);
	if (node->value)
		free(node->value);
		
	free(node);
}

