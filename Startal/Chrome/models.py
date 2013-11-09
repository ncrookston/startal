from django.db import models
from django.core.files.storage import FileSystemStorage
from Startal.Chrome.utility import to_lat_lon_str

fs = FileSystemStorage(location='Sarry/OutputFiles')

class ElevationData(models.Model):
  name = models.CharField(max_length=128)
  model = models.CharField(max_length=128)
  flags = models.TextField()
  def __unicode__(self):
    return self.name

class SarData(models.Model):
  name = models.CharField(max_length=128)
  model = models.CharField(max_length=128)
  flags = models.TextField()
  overview_kmz = models.FileField(upload_to='overview', storage=fs)

  def __unicode__(self):
    return self.name
  def associated_jobs(self):
    return processing_task.objects.filter(data=self)

class RegionOfInterest(models.Model):
  ulLat = models.FloatField()
  ulLon = models.FloatField()
  urLat = models.FloatField()
  urLon = models.FloatField()
  lrLat = models.FloatField()
  lrLon = models.FloatField()
  llLat = models.FloatField()
  llLon = models.FloatField()
  def __unicode__(self):
    return '(' + to_lat_lon_str(self.ulLat, self.ulLon) + '); ' + \
           '(' + to_lat_lon_str(self.urLat, self.urLon) + '); ' + \
           '(' + to_lat_lon_str(self.lrLat, self.lrLon) + '); ' + \
           '(' + to_lat_lon_str(self.llLat, self.llLon) + ');'

  def center_point(self):
    centerLat = (self.ulLat + self.urLat + self.lrLat + self.llLat) / 4
    centerLon = (self.ulLon + self.urLon + self.lrLon + self.llLon) / 4
    return '(' + to_lat_lon_str(centerLat, centerLon) + ')'

  def static_image_url_general(self, width, height, weight):
    return 'http://maps.googleapis.com/maps/api/staticmap' \
      + '?path=color:red|weight:' + str(weight) + '|' \
      + str(self.ulLat) + ',' + str(self.ulLon) + '|' \
      + str(self.urLat) + ',' + str(self.urLon) + '|' \
      + str(self.lrLat) + ',' + str(self.lrLon) + '|' \
      + str(self.llLat) + ',' + str(self.llLon) + '|' \
      + str(self.ulLat) + ',' + str(self.ulLon) \
      + '&maptype=satellite&sensor=false&size=' \
      + str(width) + 'x' + str(height)

  def static_image_url_small(self):
    return self.static_image_url_general(50, 50, 3)

  def static_image_url(self):
    return self.static_image_url_general(150, 150, 5)

class ProcessingTask(models.Model):
  elevation_data = models.ForeignKey(ElevationData, on_delete=models.PROTECT)
  sar_data = models.ForeignKey(SarData, on_delete=models.PROTECT)
  roi = models.ForeignKey(RegionOfInterest, on_delete=models.PROTECT)
  model = models.CharField(max_length=128)
  flags = models.TextField()
  output_kmz = models.FileField(upload_to='output', storage=fs)
  command_text = models.TextField()
  start_time = models.DateTimeField()
  stop_time = models.DateTimeField()
  output_text = models.TextField()

  def __unicode__(self):
    return str(self.id) + '_' + self.sar_data.name
  def state_description(self):
    for tup in self.STATES:
      if tup[0] == self.state:
        return tup[1]
    return 'Invalid State'

