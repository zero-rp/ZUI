#include "mxml.h"

#define mxml_bad_char(ch) ((ch) < ' ' && (ch) != '\n' && (ch) != '\r' && (ch) != '\t')

/*判断字符是否为空白字符*/
static int	mxml_isspace(int ch)
{
	return (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');
}

//--------ATTR操作
static int mxml_set_attr(mxml_node_t *node, const char  *name, char *value)
{
	int		i;			/* Looping var */
	mxml_attr_t	*attr;			/* New attribute */

	for (i = node->value.num_attrs, attr = node->value.attrs;
		i > 0;
		i--, attr++)
		if (!strcmp(attr->name, name))
		{
			/*
			* Free the old value as needed...
			*/

			if (attr->value)
				free(attr->value);

			attr->value = value;

			return (0);
		}

	if (node->value.num_attrs == 0)
		attr = (mxml_attr_t *)malloc(sizeof(mxml_attr_t));
	else
		attr = (mxml_attr_t *)realloc(node->value.attrs,
		(node->value.num_attrs + 1) * sizeof(mxml_attr_t));

	if (!attr)
	{
		return (-1);
	}

	node->value.attrs = attr;
	attr += node->value.num_attrs;

	if ((attr->name = strdup(name)) == NULL)
	{
		return (-1);
	}

	attr->value = value;

	node->value.num_attrs++;

	return (0);
}

const char *mxmlElementGetAttr(mxml_node_t *node, const char  *name)
{
	int		i;			/* Looping var */
	mxml_attr_t	*attr;			/* Cirrent attribute */


	if (!node || !name)
		return (NULL);

	for (i = node->value.num_attrs, attr = node->value.attrs;
		i > 0;
		i--, attr++)
	{

		if (!strcmp(attr->name, name))
		{
			return (attr->value);
		}
	}
	return (NULL);
}

void mxmlElementSetAttr(mxml_node_t *node, const char  *name, const char  *value)
{
	char	*valuec;			/* Copy of value */

	if (!node || !name)
		return;

	if (value)
		valuec = strdup(value);
	else
		valuec = NULL;

	if (mxml_set_attr(node, name, valuec))
		free(valuec);
}
//-------节点操作
void mxmlRemove(mxml_node_t *node)
{
	if (!node || !node->parent)
		return;

	/*
	* Remove from parent...
	*/
	if (node->prev)
		node->prev->next = node->next;
	else
		node->parent->child = node->next;

	if (node->next)
		node->next->prev = node->prev;
	else
		node->parent->last_child = node->prev;

	node->parent = NULL;
	node->prev = NULL;
	node->next = NULL;
}

void mxmlAdd(mxml_node_t *parent, int where, mxml_node_t *child, mxml_node_t *node)
{
	if (!parent || !node)
		return;

	/*
	* Remove the node from any existing parent...
	*/

	if (node->parent)
		mxmlRemove(node);

	/*
	* Reset pointers...
	*/

	node->parent = parent;

	switch (where)
	{
	case MXML_ADD_BEFORE:
		if (!child || child == parent->child || child->parent != parent)
		{
			/*
			* Insert as first node under parent...
			*/

			node->next = parent->child;

			if (parent->child)
				parent->child->prev = node;
			else
				parent->last_child = node;

			parent->child = node;
		}
		else
		{
			/*
			* Insert node before this child...
			*/

			node->next = child;
			node->prev = child->prev;

			if (child->prev)
				child->prev->next = node;
			else
				parent->child = node;

			child->prev = node;
		}
		break;

	case MXML_ADD_AFTER:
		if (!child || child == parent->last_child || child->parent != parent)
		{
			/*
			* Insert as last node under parent...
			*/

			node->parent = parent;
			node->prev = parent->last_child;

			if (parent->last_child)
				parent->last_child->next = node;
			else
				parent->child = node;

			parent->last_child = node;
		}
		else
		{
			/*
			* Insert node after this child...
			*/

			node->prev = child;
			node->next = child->next;

			if (child->next)
				child->next->prev = node;
			else
				parent->last_child = node;

			child->next = node;
		}
		break;
	}
}

static mxml_node_t *mxml_new(mxml_node_t *parent)
{
	mxml_node_t	*node;			/* New node */

	if ((node = (mxml_node_t *)calloc(1, sizeof(mxml_node_t))) == NULL)
	{
		return (NULL);
	}

	if (parent)
		mxmlAdd(parent, MXML_ADD_AFTER, MXML_ADD_TO_PARENT, node);
	//返回节点
	return (node);
}

void mxmlDelete(mxml_node_t *node)
{
	int	i;				/* Looping var */

	if (!node)
		return;

	mxmlRemove(node);

	//删除子节点
	while (node->child)
		mxmlDelete(node->child);

	if (node->value.name)
		free(node->value.name);

	if (node->value.num_attrs)
	{
		for (i = 0; i < node->value.num_attrs; i++)
		{
			if (node->value.attrs[i].name)
				free(node->value.attrs[i].name);
			if (node->value.attrs[i].value)
				free(node->value.attrs[i].value);
		}

		free(node->value.attrs);
	}
	//释放节点内存
	free(node);
}

mxml_node_t *mxmlNewElement(mxml_node_t *parent, const char  *name)
{
	mxml_node_t	*node;			/* New node */

	if (!name)
		return (NULL);

	/*
	* Create the node and set the element name...
	*/

	if ((node = mxml_new(parent)) != NULL)
		node->value.name = strdup(name);

	return (node);
}

int mxmlRelease(mxml_node_t *node)
{
	if (node)
	{
		mxmlDelete(node);
		return (0);
	}
	else
		return (-1);
}
//--------XML树解析
//取字符
static int mxml_string_getc(void *p)
{
	int		ch;			/* Character */
	const char	**s;			/* Pointer to string pointer */
	s = (const char **)p;
	if ((ch = (*s)[0] & 255) != 0)
	{
		(*s)++;
		if (!(ch & 0x80))
		{
			if (mxml_bad_char(ch))
			{
				return (EOF);
			}
			return (ch);
		}
		if (*(*s)-- < 0)
		{
			char buf[4];
			buf[0] = *(*s);
			*(*s)++;
			buf[1] = *(*s);
			buf[2] = 0;
			buf[3] = 0;
			char *a = buf;
			(*s)++;
			ch = *(int*)a;
			return (ch);
		}
		else if ((ch & 0xe0) == 0xc0)
		{
			/*
			* Two-byte value...
			*/
			if (((*s)[0] & 0xc0) != 0x80)
				return (EOF);

			ch = ((ch & 0x1f) << 6) | ((*s)[0] & 0x3f);

			(*s)++;

			if (ch < 0x80)
			{
				return (EOF);
			}
			return (ch);
		}
		else if ((ch & 0xf0) == 0xe0)
		{
			/*
			* Three-byte value...
			*/
			if (((*s)[0] & 0xc0) != 0x80 ||
				((*s)[1] & 0xc0) != 0x80)
				return (EOF);

			ch = ((((ch & 0x0f) << 6) | ((*s)[0] & 0x3f)) << 6) | ((*s)[1] & 0x3f);

			(*s) += 2;

			if (ch < 0x800)
			{
				return (EOF);
			}
			/*
			* Ignore (strip) Byte Order Mark (BOM)...
			*/
			if (ch == 0xfeff)
				return (mxml_string_getc(p));
			return (ch);
		}
		else if ((ch & 0xf8) == 0xf0)
		{
			/*
			* Four-byte value...
			*/

			if (((*s)[0] & 0xc0) != 0x80 ||
				((*s)[1] & 0xc0) != 0x80 ||
				((*s)[2] & 0xc0) != 0x80)
				return (EOF);

			ch = ((((((ch & 0x07) << 6) | ((*s)[0] & 0x3f)) << 6) |
				((*s)[1] & 0x3f)) << 6) | ((*s)[2] & 0x3f);

			(*s) += 3;

			if (ch < 0x10000)
			{
				return (EOF);
			}
			return (ch);
		}
		else
			return (EOF);
	}

	return (EOF);
}

static int mxml_add_char(int ch, char **bufptr, char **buffer, int  *bufsize)
{
	char	*newbuffer;			/* New buffer value */


	if (*bufptr >= (*buffer + *bufsize - 4))
	{
		/*
		 * Increase the size of the buffer...
		 */

		if (*bufsize < 1024)
			(*bufsize) *= 2;
		else
			(*bufsize) += 1024;

		if ((newbuffer = (char *)realloc(*buffer, *bufsize)) == NULL)
		{
			free(*buffer);
			return (-1);
		}

		*bufptr = newbuffer + (*bufptr - *buffer);
		*buffer = newbuffer;
	}

	if (ch < 0x80)
	{
		/*
		 * Single byte ASCII...
		 */
		*(*bufptr)++ = ch;
	}
	else if (ch < 0x800)
	{
		/*
		 * Two-byte UTF-8...
		 */

		*(*bufptr)++ = 0xc0 | (ch >> 6);
		*(*bufptr)++ = 0x80 | (ch & 0x3f);
	}
	else if (((char *)&ch)[0] < 0)
	{
		/*
		 * 中文
		 */
		*(*bufptr)++ = ((char *)&ch)[0];
		*(*bufptr)++ = ((char *)&ch)[1];
	}

	return (0);
}
//查找转义符
static int mxmlEntityGetValue(const char *name)
{
	int	diff,				/* Difference between names */
		current,			/* Current entity in search */
		first,				/* First entity in search */
		last;				/* Last entity in search */

	static const struct
	{
		const char	*name;			/* Entity name */
		int		val;			/* Character value */
	}	entities[] =
	{
		{ "AElig", 198 },
		{ "Aacute", 193 },
		{ "Acirc", 194 },
		{ "Agrave", 192 },
		{ "Alpha", 913 },
		{ "Aring", 197 },
		{ "Atilde", 195 },
		{ "Auml", 196 },
		{ "Beta", 914 },
		{ "Ccedil", 199 },
		{ "Chi", 935 },
		{ "Dagger", 8225 },
		{ "Delta", 916 },
		{ "Dstrok", 208 },
		{ "ETH", 208 },
		{ "Eacute", 201 },
		{ "Ecirc", 202 },
		{ "Egrave", 200 },
		{ "Epsilon", 917 },
		{ "Eta", 919 },
		{ "Euml", 203 },
		{ "Gamma", 915 },
		{ "Iacute", 205 },
		{ "Icirc", 206 },
		{ "Igrave", 204 },
		{ "Iota", 921 },
		{ "Iuml", 207 },
		{ "Kappa", 922 },
		{ "Lambda", 923 },
		{ "Mu", 924 },
		{ "Ntilde", 209 },
		{ "Nu", 925 },
		{ "OElig", 338 },
		{ "Oacute", 211 },
		{ "Ocirc", 212 },
		{ "Ograve", 210 },
		{ "Omega", 937 },
		{ "Omicron", 927 },
		{ "Oslash", 216 },
		{ "Otilde", 213 },
		{ "Ouml", 214 },
		{ "Phi", 934 },
		{ "Pi", 928 },
		{ "Prime", 8243 },
		{ "Psi", 936 },
		{ "Rho", 929 },
		{ "Scaron", 352 },
		{ "Sigma", 931 },
		{ "THORN", 222 },
		{ "Tau", 932 },
		{ "Theta", 920 },
		{ "Uacute", 218 },
		{ "Ucirc", 219 },
		{ "Ugrave", 217 },
		{ "Upsilon", 933 },
		{ "Uuml", 220 },
		{ "Xi", 926 },
		{ "Yacute", 221 },
		{ "Yuml", 376 },
		{ "Zeta", 918 },
		{ "aacute", 225 },
		{ "acirc", 226 },
		{ "acute", 180 },
		{ "aelig", 230 },
		{ "agrave", 224 },
		{ "alefsym", 8501 },
		{ "alpha", 945 },
		{ "amp", '&' },
		{ "and", 8743 },
		{ "ang", 8736 },
		{ "apos", '\'' },
		{ "aring", 229 },
		{ "asymp", 8776 },
		{ "atilde", 227 },
		{ "auml", 228 },
		{ "bdquo", 8222 },
		{ "beta", 946 },
		{ "brkbar", 166 },
		{ "brvbar", 166 },
		{ "bull", 8226 },
		{ "cap", 8745 },
		{ "ccedil", 231 },
		{ "cedil", 184 },
		{ "cent", 162 },
		{ "chi", 967 },
		{ "circ", 710 },
		{ "clubs", 9827 },
		{ "cong", 8773 },
		{ "copy", 169 },
		{ "crarr", 8629 },
		{ "cup", 8746 },
		{ "curren", 164 },
		{ "dArr", 8659 },
		{ "dagger", 8224 },
		{ "darr", 8595 },
		{ "deg", 176 },
		{ "delta", 948 },
		{ "diams", 9830 },
		{ "die", 168 },
		{ "divide", 247 },
		{ "eacute", 233 },
		{ "ecirc", 234 },
		{ "egrave", 232 },
		{ "empty", 8709 },
		{ "emsp", 8195 },
		{ "ensp", 8194 },
		{ "epsilon", 949 },
		{ "equiv", 8801 },
		{ "eta", 951 },
		{ "eth", 240 },
		{ "euml", 235 },
		{ "euro", 8364 },
		{ "exist", 8707 },
		{ "fnof", 402 },
		{ "forall", 8704 },
		{ "frac12", 189 },
		{ "frac14", 188 },
		{ "frac34", 190 },
		{ "frasl", 8260 },
		{ "gamma", 947 },
		{ "ge", 8805 },
		{ "gt", '>' },
		{ "hArr", 8660 },
		{ "harr", 8596 },
		{ "hearts", 9829 },
		{ "hellip", 8230 },
		{ "hibar", 175 },
		{ "iacute", 237 },
		{ "icirc", 238 },
		{ "iexcl", 161 },
		{ "igrave", 236 },
		{ "image", 8465 },
		{ "infin", 8734 },
		{ "int", 8747 },
		{ "iota", 953 },
		{ "iquest", 191 },
		{ "isin", 8712 },
		{ "iuml", 239 },
		{ "kappa", 954 },
		{ "lArr", 8656 },
		{ "lambda", 955 },
		{ "lang", 9001 },
		{ "laquo", 171 },
		{ "larr", 8592 },
		{ "lceil", 8968 },
		{ "ldquo", 8220 },
		{ "le", 8804 },
		{ "lfloor", 8970 },
		{ "lowast", 8727 },
		{ "loz", 9674 },
		{ "lrm", 8206 },
		{ "lsaquo", 8249 },
		{ "lsquo", 8216 },
		{ "lt", '<' },
		{ "macr", 175 },
		{ "mdash", 8212 },
		{ "micro", 181 },
		{ "middot", 183 },
		{ "minus", 8722 },
		{ "mu", 956 },
		{ "nabla", 8711 },
		{ "nbsp", 160 },
		{ "ndash", 8211 },
		{ "ne", 8800 },
		{ "ni", 8715 },
		{ "not", 172 },
		{ "notin", 8713 },
		{ "nsub", 8836 },
		{ "ntilde", 241 },
		{ "nu", 957 },
		{ "oacute", 243 },
		{ "ocirc", 244 },
		{ "oelig", 339 },
		{ "ograve", 242 },
		{ "oline", 8254 },
		{ "omega", 969 },
		{ "omicron", 959 },
		{ "oplus", 8853 },
		{ "or", 8744 },
		{ "ordf", 170 },
		{ "ordm", 186 },
		{ "oslash", 248 },
		{ "otilde", 245 },
		{ "otimes", 8855 },
		{ "ouml", 246 },
		{ "para", 182 },
		{ "part", 8706 },
		{ "permil", 8240 },
		{ "perp", 8869 },
		{ "phi", 966 },
		{ "pi", 960 },
		{ "piv", 982 },
		{ "plusmn", 177 },
		{ "pound", 163 },
		{ "prime", 8242 },
		{ "prod", 8719 },
		{ "prop", 8733 },
		{ "psi", 968 },
		{ "quot", '\"' },
		{ "rArr", 8658 },
		{ "radic", 8730 },
		{ "rang", 9002 },
		{ "raquo", 187 },
		{ "rarr", 8594 },
		{ "rceil", 8969 },
		{ "rdquo", 8221 },
		{ "real", 8476 },
		{ "reg", 174 },
		{ "rfloor", 8971 },
		{ "rho", 961 },
		{ "rlm", 8207 },
		{ "rsaquo", 8250 },
		{ "rsquo", 8217 },
		{ "sbquo", 8218 },
		{ "scaron", 353 },
		{ "sdot", 8901 },
		{ "sect", 167 },
		{ "shy", 173 },
		{ "sigma", 963 },
		{ "sigmaf", 962 },
		{ "sim", 8764 },
		{ "spades", 9824 },
		{ "sub", 8834 },
		{ "sube", 8838 },
		{ "sum", 8721 },
		{ "sup", 8835 },
		{ "sup1", 185 },
		{ "sup2", 178 },
		{ "sup3", 179 },
		{ "supe", 8839 },
		{ "szlig", 223 },
		{ "tau", 964 },
		{ "there4", 8756 },
		{ "theta", 952 },
		{ "thetasym", 977 },
		{ "thinsp", 8201 },
		{ "thorn", 254 },
		{ "tilde", 732 },
		{ "times", 215 },
		{ "trade", 8482 },
		{ "uArr", 8657 },
		{ "uacute", 250 },
		{ "uarr", 8593 },
		{ "ucirc", 251 },
		{ "ugrave", 249 },
		{ "uml", 168 },
		{ "upsih", 978 },
		{ "upsilon", 965 },
		{ "uuml", 252 },
		{ "weierp", 8472 },
		{ "xi", 958 },
		{ "yacute", 253 },
		{ "yen", 165 },
		{ "yuml", 255 },
		{ "zeta", 950 },
		{ "zwj", 8205 },
		{ "zwnj", 8204 }
	};


	/*
	* Do a binary search for the named entity...
	*/

	first = 0;
	last = (int)(sizeof(entities) / sizeof(entities[0]) - 1);

	while ((last - first) > 1)
	{
		current = (first + last) / 2;

		if ((diff = strcmp(name, entities[current].name)) == 0)
			return (entities[current].val);
		else if (diff < 0)
			last = current;
		else
			first = current;
	}

	/*
	* If we get here, there is a small chance that there is still
	* a match; check first and last...
	*/

	if (!strcmp(name, entities[first].name))
		return (entities[first].val);
	else if (!strcmp(name, entities[last].name))
		return (entities[last].val);
	else
		return (-1);
}
//处理转义符号
static int mxml_get_entity(mxml_node_t *parent,	void *p)
{
	int		ch;					/* Current character */
	char	entity[64],			/* Entity string */
			*entptr;			/* Pointer into entity */

	entptr = entity;

	while ((ch = mxml_string_getc(p)) != EOF)
		if (ch > 126 || (!isalnum(ch) && ch != '#'))
			break;
		else if (entptr < (entity + sizeof(entity) - 1))
			*entptr++ = ch;
		else
		{
			break;
		}

		*entptr = '\0';

		if (ch != ';')
		{
			return (EOF);
		}

		if (entity[0] == '#')
		{
			if (entity[1] == 'x')
				ch = strtol(entity + 2, NULL, 16);
			else
				ch = strtol(entity + 1, NULL, 10);
		}
		else
			ch = mxmlEntityGetValue(entity);

		if (mxml_bad_char(ch))
		{
			return (EOF);
		}

		return (ch);
}

static int mxml_parse_element(mxml_node_t *node, void *p)
{
	int	ch,				/* Current character in file */
		quote;				/* Quoting character */
	char	*name,				/* Attribute name */
		*value,				/* Attribute value */
		*ptr;				/* Pointer into name/value */
	int	namesize,			/* Size of name string */
		valsize;			/* Size of value string */


	/*
	* Initialize the name and value buffers...
	*/

	if ((name = (char *)malloc(64)) == NULL)
	{
		return (EOF);
	}

	namesize = 64;

	if ((value = (char *)malloc(64)) == NULL)
	{
		free(name);
		return (EOF);
	}

	valsize = 64;

	/*
	* Loop until we hit a >, /, ?, or EOF...
	*/

	while ((ch = mxml_string_getc(p)) != EOF)
	{
		if (mxml_isspace(ch))
			continue;

		/*
		* Stop at /, ?, or >...
		*/

		if (ch == '/' || ch == '?')
		{
			/*
			* Grab the > character and print an error if it isn't there...
			*/

			quote = mxml_string_getc(p);

			if (quote != '>')
			{
				goto error;
			}

			break;
		}
		else if (ch == '<')
		{
			goto error;
		}
		else if (ch == '>')
			break;

		/*
		* Read the attribute name...
		*/

		name[0] = ch;
		ptr = name + 1;

		if (ch == '\"' || ch == '\'')
		{
			/*
			* Name is in quotes, so get a quoted string...
			*/

			quote = ch;

			while ((ch = mxml_string_getc(p)) != EOF)
			{
				if (ch == '&'){
					if ((ch = mxml_get_entity(node, p)) == EOF)
						goto error;
					if (ch == ';')
						continue;//跳过分号
				}
				if (mxml_add_char(ch, &ptr, &name, &namesize))
					goto error;

				if (ch == quote)
					break;
			}
		}
		else
		{
			/*
			* Grab an normal, non-quoted name...
			*/

			while ((ch = mxml_string_getc(p)) != EOF)
				if (mxml_isspace(ch) || ch == '=' || ch == '/' || ch == '>' ||
					ch == '?')
					break;
				else
				{
					if (ch == '&'){
						if ((ch = mxml_get_entity(node, p)) == EOF)
							goto error;
						if (ch == ';')
							continue;//跳过分号
					}
					if (mxml_add_char(ch, &ptr, &name, &namesize))
						goto error;
				}
		}

		*ptr = '\0';

		if (mxmlElementGetAttr(node, name))
			goto error;

		while (ch != EOF && mxml_isspace(ch))
			ch = mxml_string_getc(p);

		if (ch == '=')
		{
			/*
			* Read the attribute value...
			*/

			while ((ch = mxml_string_getc(p)) != EOF && mxml_isspace(ch));

			if (ch == EOF)
			{
				goto error;
			}

			if (ch == '\'' || ch == '\"')
			{
				/*
				* Read quoted value...
				*/

				quote = ch;
				ptr = value;

				while ((ch = mxml_string_getc(p)) != EOF)
					if (ch == quote)
						break;
					else
					{
						if (ch == '&'){
							if ((ch = mxml_get_entity(node, p)) == EOF)
								goto error;
							if (ch == ';')
								continue;//跳过分号
						}
						if (mxml_add_char(ch, &ptr, &value, &valsize))
							goto error;
					}

				*ptr = '\0';
			}
			else
			{
				/*
				* Read unquoted value...
				*/

				value[0] = ch;
				ptr = value + 1;

				while ((ch = mxml_string_getc(p)) != EOF)
					if (mxml_isspace(ch) || ch == '=' || ch == '/' || ch == '>')
						break;
					else
					{
						if (ch == '&'){
							if ((ch = mxml_get_entity(node, p)) == EOF)
								goto error;
							if (ch == ';')
								continue;//跳过分号
						}
						if (mxml_add_char(ch, &ptr, &value, &valsize))
							goto error;
					}

				*ptr = '\0';
			}

			/*
			* Set the attribute with the given string value...
			*/

			mxmlElementSetAttr(node, name, value);
		}
		else
		{
			goto error;
		}

		/*
		* Check the end character...
		*/

		if (ch == '/' || ch == '?')
		{
			/*
			* Grab the > character and print an error if it isn't there...
			*/

			quote = mxml_string_getc(p);

			if (quote != '>')
			{
				ch = EOF;
			}

			break;
		}
		else if (ch == '>')
			break;
	}

	/*
	* Free the name and value buffers and return...
	*/

	free(name);
	free(value);

	return (ch);

error:

	free(name);
	free(value);

	return (EOF);
}
//加载XML字符串 返回XML树
mxml_node_t *mxmlLoadString(mxml_node_t *top, const char *s)
{
	mxml_node_t	*node,		/* Current node */
			*first,			/* First node added */
			*parent;		/* Current parent node */
	int		ch;				/* Non-zero if whitespace seen */
	char	*buffer,		/* String buffer */
			*bufptr;		/* Pointer into buffer */
	int		bufsize;		/* Size of buffer */
	void	*p = (void *)&s;
	if ((buffer = (char *)malloc(64)) == NULL)
	{
		return (NULL);
	}

	bufsize = 64;
	bufptr = buffer;
	parent = top;
	first = NULL;


	while ((ch = mxml_string_getc(p)) != EOF)
	{
		if ((ch == '<' || mxml_isspace(ch)) &&
			bufptr > buffer)
		{
			/*
			 * Add a new value node...
			 */

			*bufptr = '\0';

			node = mxmlNewElement(parent, buffer);

			if (*bufptr)
			{
				/*
				 * Bad integer/real number value...
				 */
				break;
			}

			bufptr = buffer;

			if (!first && node)
				first = node;
		}


		if (ch == '<')
		{
			/*
			 * Start of open/close tag...
			 */

			bufptr = buffer;

			while ((ch = mxml_string_getc(p)) != EOF)
				if (mxml_isspace(ch) || ch == '>' || (ch == '/' && bufptr > buffer))
					break;
				else if (ch == '<')
				{
					goto error;
				}
				else if (ch == '&')
				{
					if ((ch = mxml_get_entity(parent, p)) == EOF)
						goto error;
					if (ch == ';')
						continue;//跳过分号
					if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
						goto error;
				}
				else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
					goto error;
				else if (((bufptr - buffer) == 1 && buffer[0] == '?') ||
					((bufptr - buffer) == 3 && !strncmp(buffer, "!--", 3)) ||
					((bufptr - buffer) == 8 && !strncmp(buffer, "![CDATA[", 8)))
					break;

				*bufptr = '\0';

				if (!strcmp(buffer, "!--"))
				{
					/*
					 * Gather rest of comment...
					 */

					while ((ch = mxml_string_getc(p)) != EOF)
					{
						if (ch == '>' && bufptr > (buffer + 4) &&
							bufptr[-3] != '-' && bufptr[-2] == '-' && bufptr[-1] == '-')
							break;
						else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
							goto error;
					}

					/*
					 * Error out if we didn't get the whole comment...
					 */

					if (ch != '>')
					{
						/*
						 * Print error and return...
						 */
						goto error;
					}


					/*
					 * Otherwise add this as an element under the current parent...
					 */

					*bufptr = '\0';

					if (!parent && first)
					{
						/*
						 * There can only be one root element!
						 */
						goto error;
					}
					//不解析跳过注释
					node = NULL;
					/*
					if ((node = mxmlNewElement(parent, buffer)) == NULL)
					{
						break;
					}

					if (node && !first)
						first = node;
					*/
				}
				else if (!strcmp(buffer, "![CDATA["))
				{
					/*
					 * Gather CDATA section...
					 */

					while ((ch = mxml_string_getc(p)) != EOF)
					{
						if (ch == '>' && !strncmp(bufptr - 2, "]]", 2))
							break;
						else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
							goto error;
					}

					/*
					 * Error out if we didn't get the whole comment...
					 */

					if (ch != '>')
					{
						/*
						 * Print error and return...
						 */
						goto error;
					}


					/*
					 * Otherwise add this as an element under the current parent...
					 */

					*bufptr = '\0';

					if (!parent && first)
					{
						/*
						 * There can only be one root element!
						 */
						goto error;
					}

					if ((node = mxmlNewElement(parent, buffer)) == NULL)
					{
						goto error;
					}

					if (node && !first)
						first = node;
				}
				else if (buffer[0] == '?')
				{
					/*
					 * Gather rest of processing instruction...
					 */

					while ((ch = mxml_string_getc(p)) != EOF)
					{
						if (ch == '>' && bufptr > buffer && bufptr[-1] == '?')
							break;
						else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
							goto error;
					}

					/*
					 * Error out if we didn't get the whole processing instruction...
					 */

					if (ch != '>')
					{
						/*
						 * Print error and return...
						 */
						goto error;
					}

					/*
					 * Otherwise add this as an element under the current parent...
					 */

					*bufptr = '\0';

					if (!parent && first)
					{
						/*
						 * There can only be one root element!
						 */
						goto error;
					}

					if ((node = mxmlNewElement(parent, buffer)) == NULL)
					{
						/*
						 * Print error and return...
						 */
						goto error;
					}

					if (node)
					{
						if (!first)
							first = node;

						if (!parent)
						{
							parent = node;
						}
					}
				}
				else if (buffer[0] == '!')
				{
					/*
					 * Gather rest of declaration...
					 */

					do
					{
						if (ch == '>')
							break;
						else
						{
							if (ch == '&'){
								if ((ch = mxml_get_entity(parent, p)) == EOF)
									goto error;
								if (ch == ';')
									continue;//跳过分号
							}
							if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
								goto error;
						}
					} while ((ch = mxml_string_getc(p)) != EOF);

					/*
					 * Error out if we didn't get the whole declaration...
					 */

					if (ch != '>')
					{
						/*
						 * Print error and return...
						 */
						goto error;
					}

					/*
					 * Otherwise add this as an element under the current parent...
					 */

					*bufptr = '\0';

					if (!parent && first)
					{
						/*
						 * There can only be one root element!
						 */
						goto error;
					}

					if ((node = mxmlNewElement(parent, buffer)) == NULL)
					{
						goto error;
					}

					if (node)
					{
						if (!first)
							first = node;

						if (!parent)
						{
							parent = node;
						}
					}
				}
				else if (buffer[0] == '/')
				{
					/*
					 * Handle close tag...
					 */

					if (!parent || strcmp(buffer + 1, parent->value.name))
					{
						/*
						 * Close tag doesn't match tree; print an error for now...
						 */
						goto error;
					}

					/*
					 * Keep reading until we see >...
					 */

					while (ch != '>' && ch != EOF)
						ch = mxml_string_getc(p);

					node = parent;
					parent = parent->parent;

					/*
				 * Ascend into the parent and set the value type as needed...
				 */

				}
				else
				{
					/*
					 * Handle open tag...
					 */

					if (!parent && first)
					{
						/*
						 * There can only be one root element!
						 */
						goto error;
					}

					if ((node = mxmlNewElement(parent, buffer)) == NULL)
					{
						/*
						 * Just print error for now...
						 */
						goto error;
					}

					if (mxml_isspace(ch))
					{
						if ((ch = mxml_parse_element(node, p)) == EOF)
							goto error;
					}
					else if (ch == '/')
					{
						if ((ch = mxml_string_getc(p)) != '>')
						{
							mxmlDelete(node);
							goto error;
						}

						ch = '/';
					}


					if (!first)
						first = node;

					if (ch == EOF)
						break;

					if (ch != '/')
					{
						/*
						 * Descend into this node, setting the value type as needed...
						 */

						parent = node;

					}
				}

				bufptr = buffer;
		}
		else if (ch == '&')
		{
			/*
			 * Add character entity to current buffer...
			 */

			if ((ch = mxml_get_entity(parent, p)) == EOF)
				goto error;
			if (ch == ';')
				continue;//跳过分号
			if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
				goto error;
		}
		else if (!mxml_isspace(ch))
		{
			/*
			 * Add character to current buffer...
			 */

			if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
				goto error;
		}
	}

	free(buffer);

	if (parent)
	{
		node = parent;

		while (parent != top && parent->parent)
			parent = parent->parent;

		if (node != parent)
		{
			mxmlDelete(first);

			return (NULL);
		}
	}

	if (parent)
		return (parent);
	else
		return (first);
error:

	mxmlDelete(first);

	free(buffer);

	return (NULL);
}
//-------节点查找
mxml_node_t *mxmlFindElement(mxml_node_t *node, mxml_node_t *top, const char *name, const char *attr, const char *value, int descend)
{
	const char	*temp;			/* Current attribute value */


	/*
	* Range check input...
	*/

	if (!node || !top || (!attr && value))
		return (NULL);

	/*
	* Start with the next node...
	*/

	node = mxmlWalkNext(node, top, descend);

	/*
	* Loop until we find a matching element...
	*/

	while (node != NULL)
	{
		/*
		* See if this node matches...
		*/

		if (node->value.name &&
			(!name || !strcmp(node->value.name, name)))
		{
			/*
			* See if we need to check for an attribute...
			*/

			if (!attr)
				return (node);			/* No attribute search, return it... */

			/*
			* Check for the attribute...
			*/

			if ((temp = mxmlElementGetAttr(node, attr)) != NULL)
			{
				/*
				* OK, we have the attribute, does it match?
				*/

				if (!value || !strcmp(value, temp))
					return (node);		/* Yes, return it... */
			}
		}

		/*
		* No match, move on to the next node...
		*/

		if (descend == MXML_DESCEND)
			node = mxmlWalkNext(node, top, MXML_DESCEND);
		else
			node = node->next;
	}

	return (NULL);
}

mxml_node_t *mxmlFindPath(mxml_node_t *top, const char  *path)
{
	mxml_node_t	*node;			/* Current node */
	char		element[256];		/* Current element name */
	const char	*pathsep;		/* Separator in path */
	int		descend;		/* mxmlFindElement option */


	/*
	* Range check input...
	*/

	if (!top || !path || !*path)
		return (NULL);

	/*
	* Search each element in the path...
	*/

	node = top;
	while (*path)
	{
		/*
		* Handle wildcards...
		*/

		if (!strncmp(path, "*/", 2))
		{
			path += 2;
			descend = MXML_DESCEND;
		}
		else
			descend = MXML_DESCEND_FIRST;

		/*
		* Get the next element in the path...
		*/

		if ((pathsep = strchr(path, '/')) == NULL)
			pathsep = path + strlen(path);

		if (pathsep == path || (pathsep - path) >= sizeof(element))
			return (NULL);

		memcpy(element, path, pathsep - path);
		element[pathsep - path] = '\0';

		if (*pathsep)
			path = pathsep + 1;
		else
			path = pathsep;

		/*
		* Search for the element...
		*/

		if ((node = mxmlFindElement(node, node, element, NULL, NULL,
			descend)) == NULL)
			return (NULL);
	}

	/*
	* If we get this far, return the node or its first child...
	*/

	if (node->child)
		return (node->child);
	else
		return (node);
}

mxml_node_t *mxmlWalkNext(mxml_node_t *node, mxml_node_t *top, int descend)
{
	if (!node)
		return (NULL);
	else if (node->child && descend)
		return (node->child);
	else if (node == top)
		return (NULL);
	else if (node->next)
		return (node->next);
	else if (node->parent && node->parent != top)
	{
		node = node->parent;

		while (!node->next)
			if (node->parent == top || !node->parent)
				return (NULL);
			else
				node = node->parent;

		return (node->next);
	}
	else
		return (NULL);
}

mxml_node_t *mxmlWalkPrev(mxml_node_t *node, mxml_node_t *top, int descend)
{
	if (!node || node == top)
		return (NULL);
	else if (node->prev)
	{
		if (node->prev->last_child && descend)
		{
			/*
			* Find the last child under the previous node...
			*/

			node = node->prev->last_child;

			while (node->last_child)
				node = node->last_child;

			return (node);
		}
		else
			return (node->prev);
	}
	else if (node->parent != top)
		return (node->parent);
	else
		return (NULL);
}

