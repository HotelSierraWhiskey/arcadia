# Arcproject Specification v1.0

## Overview

An arcproject contains all of the information Arcadia requires to process a given CYOA story. There are three types of files in an arcproject: content files, arcfiles, and a single bookmark.json file. All arcprojects must have a .arc extention to be recognized by Arcadia.

Stories are processed by Arcadia as collections of arcfiles. Each arcfile represents one node in the directed graph representation of a CYOA story. Arcfiles define a unique node ID, a path to the content that will be presented to the reader, and any links to other nodes. This modular format is designed to scaffold writers' work so it can be efficiently processed by Arcadia and any other future development tools. All arcfiles must have a .arc.json file extension to be recognized as an arcfile. All content (.txt) files that make up a story are to be stored in the arcproject's content sub-directory. 

Beyond arcfiles, the .arc project extension, and the content directory therein, writers are encouraged to develop their own naming conventions. That is, provided the names of arcfiles and content files are unique, writers may use whatever names they see fit.

Arcadia does not parse JSONC or other supersets of JSON. Similarly, Arcadia does not parse JSON data beyond the what's documented below. A valid arcfile is a JSON file that adheres to the specifications outlined in this document.

Below is a minimal project's directory tree:
```
my_story.arc/
├── bookmark.json
├── content/
│   ├── text_0.txt
│   ├── text_1.txt
├── node_0.arc.json
├── node_1.arc.json
```

## Arcfile Structure

Each arcfile consists of the following keys:

- `content` <b>(String, Required)</b><br>
Specifies the path to the content file associated with the node relative to the content subdirectory. A content file contains the text presented to the user at this node. Only .txt files are supported.

- `choices` <b>(Object, Optional)</b><br>
Defines the available choices at this node and their corresponding links to other nodes. Keys in this object represent choice labels as strings. These keys <i>are</i> the textual representations of the choices in the CYOA. Their values are integers representing the node IDs of the linked nodes. If omitted, the node is considered an endpoint with no choices. If this key is included in an arfile, its corresponding value must be an object populated with at least one string/ integer pair. An empty object is not a valid value for this key, even if it's valid JSON.

### Arcfile Example

The node ID of an arcfile is encoded in the arcfile's name. All arcfiles must be named node_[n].arc.json where n is the UID of the node. This naming scheme makes it possible to search for nodes without opening any files. This basic arcfile example might be named node_0.arc.json. Its node ID is 0, its content is located at content/text_0.txt. All content must be located inside a subdirectory named content. Thus, only the actual name of the .txt file is required.

```json
{
	"content": "text_0.txt",
	"choices":
	{
		"choice_one": 42,
		"choice_two": 1
	}
}
```

## Bookmark Structure

Arcprojects contain exactly one top-level file named bookmark.json. This file is used to save a reader's progress as they advance through a story.

- `node` <b>(Integer, Required)</b><br>
This is the ID of the the last node where progress was saved. This allows Arcadia to save/ resume from any reachable arcfile.

- `page` <b>(Integer, Required)</b><br>
The page within the context of the node from which to resume.

### Bookmark Example

```json
{
	"node": 8,
	"page": 13
}
```

## Practical Constraints
- The total maximum length of any filename must be 128 characters, including null termination.
- No single string of non-whitespace characters can exceed the block size of an SD card (512 bytes). This includes any in-band formatting.