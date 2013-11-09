from django.shortcuts import render_to_response
from django.template import Context, loader
from django.http import HttpResponse
from Startal.Chrome.models import RegionOfInterest

def roi_tab(request):
  roi_list = RegionOfInterest.objects.all()
  return render_to_response('roi/roi_tab.html',
                            {'roi_list': roi_list,})

def add_roi(request,
            ulLat=None, ulLon=None,  urLat=None,  urLon=None,
            lrLat=None,  lrLon=None,  llLat=None, llLon=None):
  roi = RegionOfInterest(
      ulLat = ulLat,
      ulLon = ulLon,
      urLat = urLat,
      urLon = urLon,
      lrLat = lrLat,
      lrLon = lrLon,
      llLat = llLat,
      llLon = llLon)
  roi.save()
  return HttpResponse('added')

def roi_to_kml(request, roi_id = None):
  rois = RegionOfInterest.objects.filter(id = roi_id)
  if len(rois) == 0:
    return HttpResponse('not found')

  t = loader.get_template('roi/kml_file.kml')
  c = Context({'roi': rois[0]})

  response = HttpResponse(t.render(c),
      mimetype='application/vnd.google-earth.kml+xml')
  return response

def remove_roi(request, roi_id = None):
  rois = RegionOfInterest.objects.filter(id = roi_id)
  if len(rois) == 0:
    return HttpResponse('not found')

  roi = rois[0]
  roi.delete()
  return HttpResponse('deleted')

