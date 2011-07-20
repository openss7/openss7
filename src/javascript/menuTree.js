function Folder(folderDescription, hreference) //constructor
{
  //constant data
  this.desc = folderDescription
  this.hreference = hreference
  this.id = -1
  this.navObj = 0
  this.children = new Array
  this.nChildren = 0
  this.level = 0
  this.leftSideCoded = ""

  //dynamic data
  this.isOpen = false
  this.isRendered = 0

  //methods
  this.initialize = initializeFolder
  this.setState = setStateFolder
  this.addChild = addChild
  this.createIndex = createEntryIndex
  this.escondeBlock = escondeBlock
  this.esconde = escondeFolder
  this.mostra = mostra
  this.renderOb = drawFolder
  this.totalHeight = totalHeight
  this.subEntries = folderSubEntries
  this.blockStartHTML = blockStartHTML
  this.blockEndHTML = blockEndHTML
}

function initializeFolder(level, leftSide)
{
  var j=0
  var i=0
  nc = this.nChildren

  this.createIndex()
  this.level = level
  this.leftSideCoded = leftSide

  if (browserVersion == 0)
    this.isOpen=true;

  if (level>0)
    leftSide = leftSide + "0"

  if (nc > 0)
  {
    level = level + 1
    for (i=0 ; i < this.nChildren; i++)
    {
        this.children[i].initialize(level, leftSide)
    }
  }
}

function drawFolder()
{
  var docW = ""
  var leftSide = leftSideHTML(this.leftSideCoded)

  this.isRendered = 1

  if (browserVersion == 2) {
    if (!doc.yPos)
      doc.yPos=10
  }

  docW = this.blockStartHTML("folder");
  if (this.level > 0) {
    docW = docW + "<tr>"
    docW = docW + "<td class='menuCell' valign=middle nowrap width=100%>"
    docW = docW + "<a class='menuLink' href='" + this.hreference + "' TARGET=\"basefrm\"" 
    if (browserVersion > 0)
        docW = docW + "onClick='javascript:clickOnFolder("+this.id+")'"
    docW = docW + ">" + leftSide + this.desc + "&nbsp;&nbsp;" + "</a>" 
    docW = docW + "</td>"
  }
  docW = docW + this.blockEndHTML()
  doc.write(docW)

  if (browserVersion == 1)
    this.navObj = getElById("folder"+this.id)
  else if (browserVersion == 2)
  {
    this.navObj = doc.layers["folder"+this.id]
    doc.yPos=doc.yPos+this.navObj.clip.height
  }
}

function setStateFolder(isOpen)
{
  var subEntries
  var totalHeight
  var fIt = 0
  var i=0
  var currentOpen

  if (isOpen == this.isOpen)
    return

  if (browserVersion == 2)
  {
    totalHeight = 0
    for (i=0; i < this.nChildren; i++)
      totalHeight = totalHeight + this.children[i].navObj.clip.height
      subEntries = this.subEntries()
    if (this.isOpen)
      totalHeight = 0 - totalHeight
    for (fIt = this.id + subEntries + 1; fIt < nEntries; fIt++)
      indexOfEntries[fIt].navObj.moveBy(0, totalHeight)
  }
  this.isOpen = isOpen;

  propagateChangesInState(this)
}

function propagateChangesInState(folder)
{
  var i=0

  //Propagate changes
  for (i=folder.nChildren-1; i>=0; i--)
    if (folder.isOpen)
      folder.children[i].mostra()
    else
      folder.children[i].esconde()
}

function escondeFolder()
{
  this.escondeBlock()
  this.setState(0)
}

function addChild(childNode)
{
  this.children[this.nChildren] = childNode
  this.nChildren++
  return childNode
}

function folderSubEntries()
{
  var i = 0
  var se = this.nChildren

  for (i=0; i < this.nChildren; i++){
    if (this.children[i].children) //is a folder
      se = se + this.children[i].subEntries()
  }

  return se
}

// Definition of class Item (a document or link inside a Folder)
function Item(itemDescription, itemLink) // Constructor
{
  // constant data
  this.desc = itemDescription
  this.link = itemLink
  this.id = -1 //initialized in initalize()
  this.navObj = 0 //initialized in render()
  this.isRendered = 0
  this.level = 0
  this.leftSideCoded = ""
  this.nChildren = 0

  // methods
  this.initialize = initializeItem
  this.createIndex = createEntryIndex
  this.escondeBlock = escondeBlock
  this.esconde = escondeBlock
  this.mostra = mostra
  this.renderOb = drawItem
  this.totalHeight = totalHeight
  this.blockStartHTML = blockStartHTML
  this.blockEndHTML = blockEndHTML
}

function initializeItem(level, leftSide)
{
  this.createIndex()
  this.level = level
  this.leftSideCoded = leftSide
}

function drawItem()
{
  var leftSide = leftSideHTML(this.leftSideCoded)
  var docW = ""

  this.isRendered = 1

  docW = this.blockStartHTML("item")
  docW = docW + "<tr>"
  docW = docW + "<td class='menuCell' valign=middle nowrap width=100%>"
  docW = docW + "<a class='menuLink' href=" + this.link + ">" + leftSide + this.desc + "&nbsp;&nbsp;" + "</a>"
  docW = docW + "</td>"
  docW = docW + this.blockEndHTML()
  doc.write(docW)

  if (browserVersion == 2) {
    this.navObj = doc.layers["item"+this.id]
    doc.yPos=doc.yPos+this.navObj.clip.height
  } else if (browserVersion != 0) {
    this.navObj = getElById("item"+this.id)
  }
}

// Methods common to both objects (pseudo-inheritance)
function escondeBlock()
{
  if (browserVersion == 1) {
    this.navObj.style.display = "none"
  } else {
    this.navObj.visibility = "hiden"
  }
}

function mostra()
{
  if (!this.isRendered)
     this.renderOb()
  else
    if (browserVersion == 1)
      this.navObj.style.display = "block"
    else
      this.navObj.visibility = "show"
}

function blockStartHTML(idprefix) {
  var idParam = "id='" + idprefix + this.id + "'"
  var docW = ""

  if (browserVersion == 2)
    docW = "<layer "+ idParam + " top=" + doc.yPos + " >"
  else if (browserVersion == 1)
    docW = "<div " + idParam + " >"

  docW = docW + "<table border=0 cellspacing=2 cellpadding=0 width=100%>"

  return docW
}

function blockEndHTML() {
  var docW = ""

  docW = "</table>"

  if (browserVersion == 2)
    docW = docW + "</layer>"
  else if (browserVersion == 1)
    docW = docW + "</div>"

  return docW
}

function createEntryIndex()
{
  this.id = nEntries
  indexOfEntries[nEntries] = this
  nEntries++
}

// total height of subEntries open
function totalHeight() //used with browserVersion == 2
{
  var h = this.navObj.clip.height
  var i = 0

  if (this.isOpen) //is a folder and _is_ open
    for (i=0 ; i < this.nChildren; i++)
      h = h + this.children[i].totalHeight()

  return h
}

function leftSideHTML(leftSideCoded) {
    var i;
    var retStr = "";

    for (i=0; i<leftSideCoded.length; i++)
    {
        if (leftSideCoded.charAt(i) == "0")
        {
            retStr = retStr + "&nbsp;&nbsp;&nbsp;"
        }
    }
    return retStr
}

// Events
function clickOnFolder(folderId)
{
    var clicked = indexOfEntries[folderId]

    // open only current folder, and close other siblings
    var folder;
    for ( i = 1; i < nEntries; i++ ) {
        folder = indexOfEntries[i];
        if ( clicked.level == folder.level &&
             folder.isOpen == true ) {
            folder.setState(false);
            break;
        }
    }
    clicked.setState(true)
}

// Auxiliary Functions
function gFld(description, hreference)
{
  folder = new Folder(description, hreference)
  return folder
}

function gLnk(optionFlags, description, linkData)
{
  var fullLink = "";
  var targetFlag = "";
  var target = "";
  var protocol = "";

  targetFlag = optionFlags.charAt(0)
  if (targetFlag=="R")
    target = "basefrm"
  if (targetFlag=="S")
    target = "_self"

  fullLink = "'" + linkData + "' target=" + target

  linkItem = new Item(description, fullLink)
  return linkItem
}

function insFld(parentFolder, childFolder)
{
  return parentFolder.addChild(childFolder)
}

function insDoc(parentFolder, document)
{
  return parentFolder.addChild(document)
}

function renderAllTree(nodeObj) {
  var i=0;
  nodeObj.renderOb()
  for (i=0 ; i < nodeObj.nChildren; i++) 
      renderAllTree(nodeObj.children[i])
}

function hideWholeTree(nodeObj, hideThisOne, nodeObjMove) {
  var i=0;
  var heightContained=0;
  var childrenMove=nodeObjMove;

  if (hideThisOne)
    nodeObj.escondeBlock()

  if (browserVersion == 2)
    nodeObj.navObj.moveBy(0, 0-nodeObjMove)

  for (i=0 ; i < nodeObj.nChildren; i++) {
    heightContainedInChild = hideWholeTree(nodeObj.children[i], true, childrenMove)
    if (browserVersion == 2) {
      heightContained = heightContained + heightContainedInChild + nodeObj.children[i].navObj.clip.height
      childrenMove = childrenMove + heightContainedInChild
    }
  }

  return heightContained;
}

function getElById(idVal) {
  if (document.getElementById != null)
    return document.getElementById(idVal)
  if (document.all != null)
    return document.all[idVal]
  
  alert("Problem getting element by id")
  return null
}

//Other variables
indexOfEntries = new Array
nEntries = 0
browserVersion = 0 
doc = document
doc.yPos = 0

// Main function
function initializeDocument()
{
  xbDetectBrowser();
  
  foldersTree.initialize(0, "0")
  renderAllTree(foldersTree);
  if (browserVersion != 0)
    hideWholeTree(foldersTree, false, 0)

  if (browserVersion == 2)
    doc.write("<layer top=" + indexOfEntries[nEntries-1].navObj.top + ">&nbsp;</layer>")

  if (browserVersion != 0) {
    var clickedFolder
    clickedFolder = indexOfEntries[0]
    clickedFolder.setState(true)
  }
}

function xbDetectBrowser()
{
  var oldOnError = window.onerror;

  window.onerror = null;

  // work around bug in xpcdom Mozilla 0.9.1
  window.saveNavigator = window.navigator;

  var ua = window.navigator.userAgent.toLowerCase();

  if ((ua.indexOf('msie') != -1) || (ua.indexOf('konqueror') != -1) || (ua.indexOf('gecko') != -1))
  {
    browserVersion = 1;
  }
  else if ((ua.indexOf('mozilla') !=-1) && (ua.indexOf('spoofer')==-1) && (ua.indexOf('compatible') == -1) && (ua.indexOf('opera')==-1)&& (ua.indexOf('webtv')==-1) && (ua.indexOf('hotjava')==-1))
  {
    browserVersion = 2;
  }
  else
  {
    browserVersion = 0;
  }

  window.onerror = oldOnError;
}
