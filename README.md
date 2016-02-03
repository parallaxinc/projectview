# projectview

### A simple regex-based project hierarchy parser/display

## About

projectview implements project parser that is controlled by simple regular expression rules. This allows you to build a dynamic hierarchy of your software project for whatever language or application you desire, and provides a colorful, themeable tree widget for displaying it.

#### Features

* Language-independent
* Detects circular dependencies and files not found
* Totally themeable
* Searchable with the built-in search box
* Integrates with your editor

#### Screenshots

Here, projectview is showing a Spin language project.

![](https://raw.githubusercontent.com/parallaxinc/projectview/master/screenshots/projectview.png)

projectview notifies the user when there is a circular dependency in inclusion.

![](https://raw.githubusercontent.com/parallaxinc/projectview/master/screenshots/circular.png)

It will also tell you when a file was not found.

![](https://raw.githubusercontent.com/parallaxinc/projectview/master/screenshots/notfound.png)

projectview can be searched.

![](https://raw.githubusercontent.com/parallaxinc/projectview/master/screenshots/search.png)

## Usage

#### Setup

    Parser parser;
    parser.setCaseInsensitive(true);

#### Setting up search paths

To use projectview, you need to provide a filename and
additional search paths for locating local dependencies.

    parser.setFile("../../Pikemanz/00_Pikemanz.spin");
    parser.setLibraryPaths(QStringList() << "../../../");

#### Rules

To use projectview, you must build a description of the
language elements you'd like to to display. This is done using the
`Parser::Rule` structure,

A special Rule is defined for declaring file dependencies: `_includes_`.
Using this variable will result in projectview looking in the file system
for the string you extract from the source file.

Each rule can hold multiple patterns for languages that support multiple
variations of a syntax with the same meaning.

#### Adding patterns

Rules are created by passing a list of patterns to the `addRule` function,
so let's create a list of patterns.

    QList<Parser::Pattern> patterns;

Then, we create a Pattern.

    Parser::Pattern pattern;

Each `Pattern` contains a regular expression and a list
of values that describe how to extract the string.

To extract any data at all, your regular expression must contain
unnamed pattern groups to extract.

    pattern.regex = "^[ \t]*([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*=[ \t]*(.+?)('.*?)?[ \t]*$";

`capture` is a `QList<QVariant` object that can hold either `int` or
`QString`. If an int, it will capture a group from the regular expression. If
a QString, the string will be added to the result.

    pattern.capture << 1 << " returns " << 2;

Now add the pattern to the list.

    patterns.append(pattern);

#### Adding the rule

Provide a name, icon, and color for display.

    parser.addRule("constants",patterns,
        QIcon(":/icons/projectview/block-pub.png"),
        QColor("#0000FF"));

#### Displaying the tree

    parser.buildModel();
    
Now use the `getModel()` function to display your project with a 
`QTreeView`.

    QTreeView *tree= new QTreeView();
    tree->setModel(parser.getModel());
    tree->show();
