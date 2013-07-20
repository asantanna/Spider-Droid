function addEvent(event, elm, handler, bubble) {
  if (elm.addEventListener)
    elm.addEventListener(event, handler, bubble);
  else if (elm.attachEvent)
    elm.attachEvent("on"+event, handler);
}

function delEvent(event, elm, handler, bubble) {
  if (elm.removeEventListener)
    elm.removeEventListener(event, handler, bubble);
  else if (elm.detachEvent)
    elm.detachEvent("on"+event, handler);
}

function getObjectMethodClosure(object, method) {
  return function(arg) {
    return object[method](arg); 
  }
}
var element = getObjectMethodClosure(document, "createElement");
var txtNode = getObjectMethodClosure(document, "createTextNode");

/*
 * Slider
 */

// Andre: added reqLen parameter because there is no provision
// for after the fact resizing. "reqLen" is length of control
// in pixels - it will set width/height depending on "orientation"

function Slider(id, orientation, reqLen, resolution) {

  // dynamically create methods

  this.drag = function (event) {
    if (!event) var event = window.event;
    var deltaX=event.clientX - parseInt(sl.s.style.left);
    var deltaY=event.clientY - parseInt(sl.s.style.top);

    addEvent("mousemove", document, moveHandler, true);
    addEvent("mouseup", document, upHandler, true);

    function moveHandler(e) {
      var x; var y; var newvalue;
      if (!e) e=window.event;

      if (sl.orientation == "horizontal") {
        x = e.clientX - deltaX;
        if (x<0) x=0;
//        if (x>sl.width-10) x=sl.width-10;
        if (x>sl.width-1) x=sl.width-1;
        sl.s.style.left=(x) + "px";
        newvalue = parseInt(x/sl.width * sl.resolution);
      } else if (sl.orientation == "vertical") {
        y = e.clientY - deltaY;
        if (y<0) y=0;
//        if (y>sl.height-10) y=sl.height-10;
        if (y>sl.height-1) y=sl.height-1;
        sl.s.style.top=(y) + "px";
        newvalue = parseInt(y/sl.height * sl.resolution);
      }
      
      if (newvalue != sl.value) {
        sl.value = newvalue;
        sl.onChange(newvalue);
      }
    }

    function upHandler(e) {
      if (!e) e=window.event;
      delEvent("mouseup", document, upHandler, true);
      delEvent("mousemove", document, moveHandler, true);
    }
  }

  this.onChange = function (value) {
    alert("default slider.onChange() function: value = " + value);
  }

  this.setStart = function (value) {
    // Use only AFTER you defined onChange
    this.value = value;
    this.onChange(value);
    this.moveSlide(value);
  }

  this.moveSlide = function (value) {
    var length = this.height;
    var l;

    if (this.orientation == "horizontal")
      length = this.width;

    // Andre: fixed parenthesis bug here
    l = parseInt(length/this.resolution * value);

    if (this.orientation == "horizontal") {
      this.s.style.left=(l)+"px";
    } else {
      this.s.style.top=(l)+"px";
    }
  }

  this.createSlider = function() {
    
    var rel = element("div");
    rel.style.display="none";
    rel.style.position = "relative";
    rel.setAttribute("id", this.id+"_slider");

    var bar = element("div");
    bar.style.position = "absolute";
    bar.style.backgroundColor = "#ccc";
    if (this.orientation == "horizontal") {
      bar.style.left = "0px";
      bar.style.top = (parseInt(this.height/2)-1)+"px";
      bar.style.width = (this.width)+"px";
      bar.style.height = "2px";
      rel.style.height = "2px";
    } else {
      bar.style.left = (parseInt(this.width/2)-1)+"px";
      bar.style.top = "0px";
      bar.style.width = "2px";
      bar.style.height = (this.height)+"px";
      rel.style.height = (this.height)+"px";
    }
    rel.appendChild(bar);

    // Set slider image as background element to fix IE quirk
    var img = element("div");
    img.style.backgroundImage='url("images/slider.png")';
    img.style.backgroundRepeat="no-repeat";
    img.style.width="11px";
    img.style.height="10px";
    img.style.padding="0px";
    img.style.position = "absolute";
    img.style.left="0px";
    img.style.top="0px";

    img.onmousedown = this.drag;
    this.s = img;

    rel.appendChild(img);
    document.getElementById(this.id).appendChild(rel);
  }

  this.show = function() {
    this.s.parentNode.style.display="block";
  }
  
  // dynamically create/init class properties
  
  this.id = id;
  this.s = null;
  this.value = 0;
  var sl = this;

  // Andre: added this to simplify code
  
  this.orientation = (orientation != undefined) ? orientation : "horizontal";
  this.resolution = (resolution != undefined) ? resolution : 10;
  reqLen = (reqLen != undefined) ? reqLen : 100;

  if (orientation == "vertical") {
    // vertical
    this.width = 10;
    this.height = reqLen;
  } else {
    // horizontal
    this.width = reqLen;
    this.height = 10;
  }
  
  this.createSlider();
}
