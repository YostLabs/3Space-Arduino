#ifndef __TSS_EEPTS_H__
#define __TSS_EEPTS_H__

#include <stdint.h>

#define TSS_EEPTS_OUT(ptr) &(ptr)->segment_count, &(ptr)->timestamp, \
    &(ptr)->estimated_gps_longitude, &(ptr)->estimated_gps_latitude, &(ptr)->estimated_gps_altitude, \
    &(ptr)->estimated_heading_angle, &(ptr)->estimated_distance_travelled, \
    &(ptr)->estimated_distance_x, &(ptr)->estimated_distance_y, &(ptr)->estimated_distance_z, \
    &(ptr)->estimated_locomotion_mode, &(ptr)->estimated_receiver_location, \
    &(ptr)->last_event_confidence, &(ptr)->overall_confidence

struct TSS_EEPTS_Output {
	uint32_t segment_count;	//number of segments (a segment refers to a single step action)
	uint32_t timestamp;	//in microseconds, set to start of most recent estimated segment
	double estimated_gps_longitude;	//in decimal degrees (positive east, negative west)
	double estimated_gps_latitude;	//in decimal degrees (positive north, negative south)
	float estimated_gps_altitude;	//in meters
	float estimated_heading_angle;	//in degrees (0 north, 90 east, 180 south, 270 west)
	float estimated_distance_travelled;	//in meters, since last reset
	float estimated_distance_x;	//in meters since last update, along positive east/negative west
	float estimated_distance_y;	//in meters, since last update, along positive north/negative south
	float estimated_distance_z;	//in meters, since last update, along positive up/negative down
	uint8_t estimated_locomotion_mode;	// 0=idle, 1=walking, etc...
	uint8_t estimated_receiver_location;	//0=unknown, 1=chest, etc...
	float last_event_confidence; //0.0(low) 1.0(high) confidence in last event.
	float overall_confidence; //0.0(low) 1.0(high) average confidence of all events.
};

#endif /* __TSS_EEPTS_H__ */