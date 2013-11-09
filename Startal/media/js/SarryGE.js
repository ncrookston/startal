google.load("earth", "1");

var SarryGE = {

init: function(id, storeBoxCb)
{
  var that = this;
  google.earth.createInstance(id, function(pluginInstance)
  {
    that.ge = pluginInstance;
    that.onSuccess(storeBoxCb);
  },
  function(errorCode)
  {
    alert('Failure: ' + errorCode);
  });
},

getLatLon: function(x, y)
{
  return this.ge.getView().hitTest(x, this.ge.UNITS_PIXELS,
                                   y, this.ge.UNITS_PIXELS,
                                   this.ge.HIT_TEST_TERRAIN);
},


beginDrag: function(event, boxPM, storeBoxCb)
{
  if(!event.getAltKey())
    return true;

  var startX = event.getClientX();
  var startY = event.getClientY();
  var endX = startX;
  var endY = startY;

  var mouseUpFunc = function (event)
  {
    endX = event.getClientX();
    endY = event.getClientY();

    drawLine(boxPM);

    google.earth.removeEventListener(
        SarryGE.ge.getWindow(),'mousemove', mouseMoveFunc);
    google.earth.removeEventListener(
        SarryGE.ge.getWindow(),'mouseup', mouseUpFunc);

    var ptUL = SarryGE.getLatLon(startX, startY);
    var ptUR = SarryGE.getLatLon(endX, startY);
    var ptLR = SarryGE.getLatLon(endX, endY);
    var ptLL = SarryGE.getLatLon(startX, endY);

    storeBoxCb(ptUL, ptUR, ptLR, ptLL);

    SarryGE.ge.getFeatures().removeChild(boxPM);
    event.preventDefault();
  }

  //Create a new KML object.  Where to store it?
  google.earth.addEventListener(this.ge.getWindow(), 'mouseup', mouseUpFunc);

  var mouseMoveFunc = function (event)
  {
    endX = event.getClientX();
    endY = event.getClientY();

    drawLine(boxPM);

    event.preventDefault();
  }

  google.earth.addEventListener(
      this.ge.getWindow(), 'mousemove', mouseMoveFunc);

  var drawLine = function(pm)
  {
    var ptUL = SarryGE.getLatLon(startX, startY);
    var ptUR = SarryGE.getLatLon(endX, startY);
    var ptLR = SarryGE.getLatLon(endX, endY);
    var ptLL = SarryGE.getLatLon(startX, endY);

    if(ptUL != null && ptUR != null && ptLR != null && ptLL != null)
    {
      var linearRing = SarryGE.ge.createLinearRing('');

      pm.setGeometry(linearRing);

      linearRing.setTessellate(true);
      //linearRing.setAltitudeMode(ge.ALTITUDE_RELATIVE_TO_GROUND);

      linearRing.getCoordinates().pushLatLngAlt(ptUL.getLatitude(), ptUL.getLongitude(), 0.);
      linearRing.getCoordinates().pushLatLngAlt(ptUR.getLatitude(), ptUR.getLongitude(), 0.);
      linearRing.getCoordinates().pushLatLngAlt(ptLR.getLatitude(), ptLR.getLongitude(), 0.);
      linearRing.getCoordinates().pushLatLngAlt(ptLL.getLatitude(), ptLL.getLongitude(), 0.);

      SarryGE.ge.getFeatures().appendChild(pm);
    }
  }

  event.preventDefault();
},

onSuccess: function(storeBoxCb)
{
  this.ge.getWindow().setVisibility(true);

  //add a navigation control
  this.ge.getNavigationControl().setVisibility(false);

  //add some layers
  this.ge.getLayerRoot().enableLayerById(this.ge.LAYER_BORDERS, true);

  var boxPM = this.ge.createPlacemark('');
  boxPM.setStyleSelector(this.ge.createStyle(''));
  var lineStyle = boxPM.getStyleSelector().getLineStyle();
  lineStyle.setWidth(5);
  lineStyle.getColor().set('550000ff');

  google.earth.addEventListener(this.ge.getGlobe(), 'mousedown',
     function(event) { SarryGE.beginDrag(event, boxPM, storeBoxCb); });
},

};

