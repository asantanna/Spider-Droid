//
// PHI web admin common routines
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



