from datetime import datetime
from django.core.files.base import ContentFile
from django.shortcuts import render_to_response
from django.template import Context, loader
from django.http import HttpResponse
from Startal.Chrome.models import ElevationData, ProcessingTask, RegionOfInterest, SarData
from Startal.Chrome.utility import get_registered_types, get_sarry_command, get_sarry_output, trim_options
from string import split
from uuid import uuid4

def index(request):
  return render_to_response('index.html')

def task_tab(request):
  return render_to_response('task/task_tab.html',
    {
      'all_tasks': ProcessingTask.objects.all(),
      'sar_data_list': SarData.objects.all(),
      'roi_list': RegionOfInterest.objects.all(),
      'elevation_list': ElevationData.objects.all(),
      'algorithm_list': get_registered_types('algorithm'),
    })

def add_task(request, sar_data_id = None, roi_id = None, elevation_id = None,
    algorithm_type = None, algorithm_flags = None):

  elevation_data = ElevationData.objects.filter(id=elevation_id)
  if len(elevation_data) == 0:
    return HttpResponse('invalid elevation data')

  sar_data = SarData.objects.filter(id=sar_data_id)
  if len(sar_data) == 0:
    return HttpResponse('invalid sar data')

  rois = RegionOfInterest.objects.filter(id=roi_id)
  if len(rois) == 0:
    return HttpResponse('invalid roi')

  flags = split(elevation_data[0].flags) \
      + split(sar_data[0].flags) + split(algorithm_flags) \
      + ['--ul-lat', str(rois[0].ulLat), '--ul-lon', str(rois[0].ulLon),
         '--ur-lat', str(rois[0].urLat), '--ur-lon', str(rois[0].urLon),
         '--lr-lat', str(rois[0].lrLat), '--lr-lon', str(rois[0].lrLon),
         '--ll-lat', str(rois[0].llLat), '--ll-lon', str(rois[0].llLon)]
  command = get_sarry_command(sar_data[0].model, elevation_data[0].model,
      algorithm_type, flags)

  data = ProcessingTask(
    elevation_data = elevation_data[0],
    sar_data = sar_data[0],
    roi = rois[0],
    model = algorithm_type,
    flags = algorithm_flags,
    command_text = " ".join(command),
    start_time = datetime.now())
  data.save()

  (sod, sed) = get_sarry_output(sar_data[0].model, elevation_data[0].model,
      algorithm_type, flags)

  filename = str(uuid4()) + '.kmz'
  content_file = ContentFile(sod)

  data.output_kmz.save(filename, content_file)
  data.stop_time = datetime.now()
  data.output_text = sed
  data.save()
  return HttpResponse(sed)

def get_algorithm_params(request, algorithm_type = None, antenna_id = None,
    elevation_id = None):
  elevation_data = ElevationData.objects.filter(id = elevation_id)
  if len(elevation_data) == 0:
    return HttpResponse('Invalid elevation data')

  sar_data = SarData.objects.filter(id = antenna_id)
  if len(sar_data) == 0:
    return HttpResponse('Invalid sar data')

  (sod, sed) = get_sarry_output(sar_data[0].model, elevation_data[0].model,
      algorithm_type, split(sar_data[0].flags) + split(elevation_data[0].flags)
      + ['--help-algorithm',])
  return HttpResponse(trim_options(sed, [('--ul-l', 1), ('--ur-l', 1),
        ('--lr-l', 1), ('--ll-l', 1)]))

def remove_task(request, task_id = None):
  jobs = ProcessingTask.objects.filter(id=task_id)
  if len(jobs) == 0:
    return HttpResponse('invalid id')
  job[0].delete()
  return HttpResponse('deleted')

