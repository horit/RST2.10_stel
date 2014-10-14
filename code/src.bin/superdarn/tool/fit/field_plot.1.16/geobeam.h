/* geobeam.h
   ========= */


struct GeoLocBeam {
  int bm;
  int frang;
  int rsep;
  int rxrise;
  float glat[MAX_RANGE+1][3];
  float glon[MAX_RANGE+1][3];
  float mlat[MAX_RANGE+1][3];
  float mlon[MAX_RANGE+1][3];
  float gazm[MAX_RANGE];
  float mazm[MAX_RANGE];
};

struct GeoLocScan {
  int num;
  struct GeoLocBeam *bm;
};
  
int GeoLocCenter(struct RadarSite *site,int mag,float *lat,float *lon);
int GeoLocBeam(struct RadarSite *site,int year,
               struct GeoLocScan *geol,struct RadarBeam *bm);
