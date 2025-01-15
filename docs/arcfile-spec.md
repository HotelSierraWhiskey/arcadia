# Arcfile Specification v1.0

Arcadia processes stories as collections of arcfiles. Each arcfile represents one node in the directed graph representation of a CYOA story. Arcfiles define a unique node ID, a path to the content that will be presented to the reader, and any links to other nodes. This modular format is designed to scaffold writers' work so it can be efficiently processed by Arcadia and any other future development tools. While arcfiles adhere to a broad specification, they are a generic, and writers are encouraged to develop and maintain their own conventions within this framework. All arcfiles must have a .arc.json file extension to be recognized by Arcadia. Additionally, arcfiles must be stored within directories designated as arcfolders, which are directories with a .arc extension.

Arcadia does not parse JSONC or other supersets of JSON. Similarly, Arcadia does parse JSON data beyond the keys documented below. A valid arcfile is a JSON file that adheres to the specifications outlined in this document.

## Structure

Each arcfile consists of the following keys:

- `id` <b>(Integer, Required)</b><br>
Represents the unique identifier for the node. Must be a non-negative integer. ID values do not need to be sequential, however they must be unique.

- `content` <b>(String, Required)</b><br>
Specifies the path to the content file associated with the node relative to the content subdirectory. A content file contains the text presented to the user at this node. Only .txt files are supported.

- `choices` <b>(Object, Optional)</b><br>
Defines the available choices at this node and their corresponding links to other nodes. Keys in this object represent choice labels as strings. These keys <i>are</i> the textual representations of the choices in the CYOA. Their values are integers representing the node IDs of the linked nodes. If omitted, the node is considered an endpoint with no choices. If this key is included in an arfile, its corresponding value must be an object populated with at least one string/ integer pair. An empty object is not a valid value for this key, even if it's valid JSON.

### Arcfile Example

This basic arcfile example might be named node_0.arc.json. Its node ID is 0, its content is located at content/text_0.txt. All content must be located inside a subdirectory named content. Thus, only the actual name of the .txt file is required.

```json
{
	"id": 0,
	"content": "text_0.txt",
	"choices":
	{
		"choice_one": 42,
		"choice_two": 1
	}
}
```

### Complete Arcadia Project Example Tree
Below is a minimal project's directory tree.
```
my_story.arc/
├── content/
│   ├── text_0.txt
│   ├── text_1.txt
├── node_0.arc.json
├── node_1.arc.json
```