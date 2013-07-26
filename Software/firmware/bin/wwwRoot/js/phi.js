//
// PHI web admin common routines
//

//
// PHI command helpers
//

function phi_getInitState() {
  sendJsonRequest("req : [ {cmd:getInitState} ]");
}

function phi_initPeripherals() {
  sendJsonRequest("req : [ {cmd:initPeripherals} ]");
}

function phi_setPower(motorId, percent) {
  sendJsonRequest("req : [ {cmd:setPower, motorId:" + motorId + ", power:" + percent + " } ]");
}

//
// Utils
//

function getPrettyByteString(byteVal) {
  var kVal = Math.floor(byteVal / 1024);
  var mVal = Math.floor(kVal / 1024);
  var gVal = Math.floor(mVal / 1024);
  if (gVal > 0) {
    mVal = mVal % 1024;
    return gVal + (mVal / 1024.0).toFixed(2).substr(1) + " GB";
  } 
  kVal = kVal % 1024;
  return mVal + (kVal / 1024.0).toFixed(2).substr(1) + " MB";
}

function getPrettyTime(secs) {
  var mins = Math.floor(secs / 60);
  secs = secs % 60;
  var hrs = Math.floor(mins / 60);
  mins = mins % 60;
  var days = Math.floor(hrs / 24);
  hrs = hrs % 24;
  var s = "";
  if (days > 0) {
    s += days + " day(s) ";
  }
  s += zeroPad(hrs,2) + ":";
  s += zeroPad(mins,2) + ":";
  s += zeroPad(secs,2);
  return s;
}

function zeroPad(n, pad) {
  return (n/Math.pow(10.0,pad)).toFixed(pad).substr(2);
}

//
// JSON common functions
//

function jsonReplyListener () {

  // alert("received JSON reply");

  if (this.readyState === 4) {
    if (this.status === 200) {
      // alert(this.responseText);
      var obj = eval ("(" + this.responseText + ")");
      if (obj == null) {
        alert("jsonReplyListener: JSON obj eval failed");
      }
      parseJsonReply(obj);
    } else {
      alert('jsonReplyListener: There was a problem with the JSON request.');
    }
  }
}

function sendJsonRequest(json) {

  // alert("sending JSON request:" + json);

  var httpRequest;

  if (window.XMLHttpRequest) {
          // Mozilla, Safari, ...
    httpRequest = new XMLHttpRequest();
  } else if (window.ActiveXObject) {
          // IE
    try {
      httpRequest = new ActiveXObject("Msxml2.XMLHTTP");
    } 
    catch (e) {
      try {
        httpRequest = new ActiveXObject("Microsoft.XMLHTTP");
      } 
      catch (e) {}
    }
  }

  if (!httpRequest) {
    alert('error: cannot create an XMLHTTP instance');
    return false;
  }

  httpRequest.onreadystatechange = jsonReplyListener;

  httpRequest.open('POST', "postJson");
  httpRequest.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  httpRequest.setRequestHeader("Content-length", json.length);
  httpRequest.setRequestHeader("Connection", "close");
  httpRequest.setRequestHeader("Content-type", "application/json");

  httpRequest.send(json);
}



