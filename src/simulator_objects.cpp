#include <simulator_objects.hpp>
#include <dataref_helpers.hpp>

#include <glm/common.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <XPLMDisplay.h>
#include <XPLMutilities.h>
#include <MoonPhase.hpp>

#define WIND_LAYER_COUNT 3

#define MPS_PER_KNOTS 0.514444444f

namespace simulator_objects
{
	XPLMDataRef viewport_dataref;
	XPLMDataRef current_eye_dataref;

	XPLMDataRef rendering_resolution_ratio_dataref;
	XPLMDataRef skip_fragments_dataref;

	XPLMDataRef reverse_z_dataref;

	XPLMDataRef modelview_matrix_dataref;
	XPLMDataRef projection_matrix_dataref;

	XPLMDataRef cloud_map_scale_dataref;

	XPLMDataRef base_noise_scale_dataref;
	XPLMDataRef detail_noise_scale_dataref;

	XPLMDataRef blue_noise_scale_dataref;

	XPLMDataRef cloud_type_datarefs[CLOUD_LAYER_COUNT];

	XPLMDataRef cloud_base_datarefs[CLOUD_LAYER_COUNT];
	XPLMDataRef cloud_top_datarefs[CLOUD_TYPE_COUNT];

	XPLMDataRef cloud_coverage_datarefs[CLOUD_TYPE_COUNT];
	XPLMDataRef cloud_density_datarefs[CLOUD_TYPE_COUNT];

	XPLMDataRef base_noise_ratio_datarefs[CLOUD_TYPE_COUNT];
	XPLMDataRef detail_noise_ratio_datarefs[CLOUD_TYPE_COUNT];

	XPLMDataRef wind_altitude_datarefs[WIND_LAYER_COUNT];

	XPLMDataRef wind_direction_datarefs[WIND_LAYER_COUNT];
	XPLMDataRef wind_speed_datarefs[WIND_LAYER_COUNT];

	XPLMDataRef zulu_time_dataref;

	XPLMDataRef fade_start_distance_dataref;
	XPLMDataRef fade_end_distance_dataref;
	
	XPLMDataRef sun_pitch_dataref;
	XPLMDataRef sun_heading_dataref;
	XPLMDataRef moon_pitch_dataref;
	XPLMDataRef moon_heading_dataref;

	XPLMDataRef sun_tint_red_dataref;
	XPLMDataRef sun_tint_green_dataref;
	XPLMDataRef sun_tint_blue_dataref;

	XPLMDataRef sun_gain_dataref;
	XPLMDataRef moon_gain_dataref;
	XPLMDataRef moon_glow_dataref;

	XPLMDataRef ambient_tint_red_dataref;
	XPLMDataRef ambient_tint_green_dataref;
	XPLMDataRef ambient_tint_blue_dataref;

	XPLMDataRef ambient_gain_dataref;

	XPLMDataRef forward_mie_scattering_dataref;
	XPLMDataRef backward_mie_scattering_dataref;

	XPLMDataRef atmosphere_bottom_tint_dataref;
	XPLMDataRef atmosphere_top_tint_dataref;

	XPLMDataRef atmospheric_blending_dataref;
	
	XPLMDataRef fog_be_gone_dataref;
	XPLMDataRef bicubic_sampling_dataref;

	XPLMDataRef day_numeric_dataref;
	XPLMDataRef month_numeric_dataref;
	XPLMDataRef moon_phase_dataref;

	XPLMDataRef matts_fdebug_dataref;
	XPLMDataRef matts_idebug_dataref;

	float previous_zulu_time;
	float current_zulu_time;

	const int sample_count = 50;
	float time_difference_combined[sample_count];
	float time_difference_average;
	int initial_sample_done;
	int combined_count;
	int time_accelerated;

	int version;

	glm::ivec4 previous_viewport;
	glm::ivec4 current_viewport;

	glm::ivec2 previous_rendering_resolution;
	glm::ivec2 current_rendering_resolution;

	int skip_fragments;
	int frame_index;

	int reverse_z;

	float near_clip_z;
	float far_clip_z;

	glm::mat4 inverse_projection_matrix;
	glm::mat4 inverse_modelview_matrix;

	float cloud_map_scale;

	float base_noise_scale;
	float detail_noise_scale;

	float blue_noise_scale;

	int cloud_types[CLOUD_LAYER_COUNT];

	float cloud_bases[CLOUD_LAYER_COUNT];
	float cloud_tops[CLOUD_LAYER_COUNT];
	
	float cloud_coverages[CLOUD_TYPE_COUNT];
	float cloud_densities[CLOUD_TYPE_COUNT];

	glm::vec3 base_noise_ratios[CLOUD_TYPE_COUNT];
	glm::vec3 detail_noise_ratios[CLOUD_TYPE_COUNT];

	glm::vec3 wind_offsets[CLOUD_LAYER_COUNT];

	float fade_start_distance;
	float fade_end_distance;

	glm::vec3 sun_direction;
	glm::vec3 moon_direction;

	glm::vec3 sun_tint;
	float sun_gain;
	float moon_gain;
	float moon_glow;

	glm::vec3 ambient_tint;
	float ambient_gain;

	float backward_mie_scattering;
	float forward_mie_scattering;

	glm::vec3 atmosphere_bottom_tint;
	glm::vec3 atmosphere_top_tint;

	float atmospheric_blending;
	float fog_be_gone;
	int bicubic_sampling;

	int day_numeric;
	int month_numeric;
	int check_day_numeric;
	int check_month_numeric;
	int year_numeric = 2020;		// need to set to system year
	float moon_phase;
	int moon_phase_percent;

	void initialize()
	{
		XPLMDataRef override_clouds_dataref = XPLMFindDataRef("sim/operation/override/override_clouds");
		XPLMSetDatai(override_clouds_dataref, 1);

		XPLMDataRef version_dataref = XPLMFindDataRef("sim/version/xplane_internal_version");
		version = XPLMGetDatai(version_dataref);

		viewport_dataref = XPLMFindDataRef("sim/graphics/view/viewport");
		current_eye_dataref = XPLMFindDataRef("sim/graphics/view/draw_call_type");

		rendering_resolution_ratio_dataref = export_float_dataref("enhanced_cloudscapes/rendering_resolution_ratio", 0.66f);
		skip_fragments_dataref = export_int_dataref("enhanced_cloudscapes/skip_fragments", 0);

		reverse_z_dataref = XPLMFindDataRef("sim/graphics/view/is_reverse_float_z");

		modelview_matrix_dataref = XPLMFindDataRef("sim/graphics/view/world_matrix");
		projection_matrix_dataref = XPLMFindDataRef("sim/graphics/view/projection_matrix");

		cloud_map_scale_dataref = export_float_dataref("enhanced_cloudscapes/cloud_map_scale", 0.000005f);

		base_noise_scale_dataref = export_float_dataref("enhanced_cloudscapes/base_noise_scale", 0.00003f);
		detail_noise_scale_dataref = export_float_dataref("enhanced_cloudscapes/detail_noise_scale", 0.0004f);

		blue_noise_scale_dataref = export_float_dataref("enhanced_cloudscapes/blue_noise_scale", 0.01f);

		cloud_type_datarefs[0] = XPLMFindDataRef("sim/weather/cloud_type[0]");
		cloud_type_datarefs[1] = XPLMFindDataRef("sim/weather/cloud_type[1]");
		cloud_type_datarefs[2] = XPLMFindDataRef("sim/weather/cloud_type[2]");

		cloud_base_datarefs[0] = XPLMFindDataRef("sim/weather/cloud_base_msl_m[0]");
		cloud_base_datarefs[1] = XPLMFindDataRef("sim/weather/cloud_base_msl_m[1]");
		cloud_base_datarefs[2] = XPLMFindDataRef("sim/weather/cloud_base_msl_m[2]");

		cloud_top_datarefs[0] = XPLMFindDataRef("sim/weather/cloud_tops_msl_m[0]");
		cloud_top_datarefs[1] = XPLMFindDataRef("sim/weather/cloud_tops_msl_m[1]");
		cloud_top_datarefs[2] = XPLMFindDataRef("sim/weather/cloud_tops_msl_m[2]");

		cloud_coverage_datarefs[0] = export_float_dataref("enhanced_cloudscapes/cirrus/coverage", 0.85f);
		cloud_coverage_datarefs[1] = export_float_dataref("enhanced_cloudscapes/scattered/coverage", 0.75f);
		cloud_coverage_datarefs[2] = export_float_dataref("enhanced_cloudscapes/broken/coverage", 0.85f);
		cloud_coverage_datarefs[3] = export_float_dataref("enhanced_cloudscapes/overcast/coverage", 0.95f);
		cloud_coverage_datarefs[4] = export_float_dataref("enhanced_cloudscapes/stratus/coverage", 1.0f);

		cloud_density_datarefs[0] = export_float_dataref("enhanced_cloudscapes/cirrus/density_multiplier", 0.002); // 0.00055);
		cloud_density_datarefs[1] = export_float_dataref("enhanced_cloudscapes/scattered/density_multiplier", 0.0035);
		cloud_density_datarefs[2] = export_float_dataref("enhanced_cloudscapes/broken/density_multiplier", 0.004);
		cloud_density_datarefs[3] = export_float_dataref("enhanced_cloudscapes/overcast/density_multiplier", 0.004);
		cloud_density_datarefs[4] = export_float_dataref("enhanced_cloudscapes/stratus/density_multiplier", 0.0045);

		base_noise_ratio_datarefs[0] = export_vec3_dataref("enhanced_cloudscapes/cirrus/base_noise_ratios", glm::vec3(0.625f, 0.25f, 0.125f));
		base_noise_ratio_datarefs[1] = export_vec3_dataref("enhanced_cloudscapes/scattered/base_noise_ratios", glm::vec3(0.625f, 0.25f, 0.125f));
		base_noise_ratio_datarefs[2] = export_vec3_dataref("enhanced_cloudscapes/broken/base_noise_ratios", glm::vec3(0.625f, 0.25f, 0.125f));
		base_noise_ratio_datarefs[3] = export_vec3_dataref("enhanced_cloudscapes/overcast/base_noise_ratios", glm::vec3(0.625f, 0.25f, 0.125f));
		base_noise_ratio_datarefs[4] = export_vec3_dataref("enhanced_cloudscapes/stratus/base_noise_ratios", glm::vec3(0.625f, 0.25f, 0.125f));

		detail_noise_ratio_datarefs[0] = export_vec3_dataref("enhanced_cloudscapes/cirrus/detail_noise_ratios", glm::vec3(0.25f, 0.125f, 0.0625f));
		detail_noise_ratio_datarefs[1] = export_vec3_dataref("enhanced_cloudscapes/scattered/detail_noise_ratios", glm::vec3(0.625f, 0.25f, 0.125f));
		detail_noise_ratio_datarefs[2] = export_vec3_dataref("enhanced_cloudscapes/broken/detail_noise_ratios", glm::vec3(0.625f, 0.25f, 0.125f));
		detail_noise_ratio_datarefs[3] = export_vec3_dataref("enhanced_cloudscapes/overcast/detail_noise_ratios", glm::vec3(0.625f, 0.25f, 0.125f));
		detail_noise_ratio_datarefs[4] = export_vec3_dataref("enhanced_cloudscapes/stratus/detail_noise_ratios", glm::vec3(0.625f, 0.25f, 0.125f));

		wind_altitude_datarefs[0] = XPLMFindDataRef("sim/weather/wind_altitude_msl_m[0]");
		wind_altitude_datarefs[1] = XPLMFindDataRef("sim/weather/wind_altitude_msl_m[1]");
		wind_altitude_datarefs[2] = XPLMFindDataRef("sim/weather/wind_altitude_msl_m[2]");

		wind_direction_datarefs[0] = XPLMFindDataRef("sim/weather/wind_direction_degt[0]");
		wind_direction_datarefs[1] = XPLMFindDataRef("sim/weather/wind_direction_degt[1]");
		wind_direction_datarefs[2] = XPLMFindDataRef("sim/weather/wind_direction_degt[2]");

		wind_speed_datarefs[0] = XPLMFindDataRef("sim/weather/wind_speed_kt[0]");
		wind_speed_datarefs[1] = XPLMFindDataRef("sim/weather/wind_speed_kt[1]");
		wind_speed_datarefs[2] = XPLMFindDataRef("sim/weather/wind_speed_kt[2]");

		zulu_time_dataref = XPLMFindDataRef("sim/time/zulu_time_sec");

		sun_pitch_dataref = XPLMFindDataRef("sim/graphics/scenery/sun_pitch_degrees");
		sun_heading_dataref = XPLMFindDataRef("sim/graphics/scenery/sun_heading_degrees");
		moon_pitch_dataref = XPLMFindDataRef("sim/graphics/scenery/moon_pitch_degrees");
		moon_heading_dataref = XPLMFindDataRef("sim/graphics/scenery/moon_heading_degrees");

		sun_gain_dataref = export_float_dataref("enhanced_cloudscapes/sun_gain", 2.5f);
		moon_gain_dataref = export_float_dataref("enhanced_cloudscapes/moon_gain", 2.0f);

		ambient_tint_red_dataref = XPLMFindDataRef("sim/graphics/misc/outside_light_level_r");
		ambient_tint_green_dataref = XPLMFindDataRef("sim/graphics/misc/outside_light_level_g");
		ambient_tint_blue_dataref = XPLMFindDataRef("sim/graphics/misc/outside_light_level_b");

		ambient_gain_dataref = export_float_dataref("enhanced_cloudscapes/ambient_gain", 0.8f);

		forward_mie_scattering_dataref = export_float_dataref("enhanced_cloudscapes/forward_mie_scattering", 0.85f);
		backward_mie_scattering_dataref = export_float_dataref("enhanced_cloudscapes/backward_mie_scattering", 0.45f);

		atmosphere_bottom_tint_dataref = export_vec3_dataref("enhanced_cloudscapes/atmosphere_bottom_tint", glm::vec3(0.55f, 0.775f, 1.0f));
		atmosphere_top_tint_dataref = export_vec3_dataref("enhanced_cloudscapes/atmosphere_top_tint", glm::vec3(0.45f, 0.675f, 1.0f));

		atmospheric_blending_dataref = export_float_dataref("enhanced_cloudscapes/atmospheric_blending", 0.45f);

		bicubic_sampling_dataref = export_int_dataref("enhanced_cloudscapes/bicubic_sampling", 0);

		day_numeric_dataref = XPLMFindDataRef("sim/cockpit2/clock_timer/current_day");
		month_numeric_dataref = XPLMFindDataRef("sim/cockpit2/clock_timer/current_month");
		day_numeric = XPLMGetDatai(day_numeric_dataref);
		month_numeric = XPLMGetDatai(month_numeric_dataref);
		check_day_numeric = XPLMGetDatai(day_numeric_dataref);
		check_month_numeric = XPLMGetDatai(month_numeric_dataref);
		moon_phase = MoonPhase::GetPhase(day_numeric, month_numeric, year_numeric);
		moon_phase_dataref = export_float_dataref("enhanced_cloudscapes/moon_phase", moon_phase);
		moon_phase_percent = MoonPhase::GetPercentage(moon_phase);
		moon_glow_dataref = export_float_dataref("enhanced_cloudscapes/moon_glow", moon_phase_percent);

		matts_fdebug_dataref = export_float_dataref("matts_debug/float_debug", 0);
		matts_idebug_dataref = export_int_dataref("matts_debug/int_debug", 0);

		for (int a = 0; a < sample_count; a++) time_difference_combined[a] = 0.0303f;

	}

	void update()
	{
		XPLMDataRef fog_clip_scale_dataref = XPLMFindDataRef("sim/private/controls/terrain/fog_clip_scale");
		XPLMSetDataf(fog_clip_scale_dataref, -100.0);

		int eye_index = XPLMGetDatai(current_eye_dataref);

		previous_viewport = current_viewport;
		XPLMGetDatavi(viewport_dataref, glm::value_ptr(current_viewport), 0, current_viewport.length());

		current_viewport.z -= current_viewport.x;
		current_viewport.w -= current_viewport.y;

		if (eye_index == 4) current_viewport.x += current_viewport.z;

		int screen_width;
		int screen_height;

		XPLMGetScreenSize(&screen_width, &screen_height);

		float rendering_resolution_ratio = XPLMGetDataf(rendering_resolution_ratio_dataref);

		previous_rendering_resolution = current_rendering_resolution;
		current_rendering_resolution = glm::ivec2(screen_width * rendering_resolution_ratio, screen_height * rendering_resolution_ratio);

		if ((eye_index == 3) || (eye_index == 4)) skip_fragments = 0;
		else skip_fragments = XPLMGetDatai(skip_fragments_dataref);

		frame_index++;

		reverse_z = XPLMGetDatai(reverse_z_dataref);

		if (reverse_z == 0)
		{
			near_clip_z = -1.0;
			far_clip_z = 1.0;
		}
		else
		{
			near_clip_z = 1.0;
			far_clip_z = 0.0;
		}

		glm::mat4 modelview_matrix;
		glm::mat4 projection_matrix;

		XPLMGetDatavf(modelview_matrix_dataref, glm::value_ptr(modelview_matrix), 0, modelview_matrix.length() * modelview_matrix.length());
		XPLMGetDatavf(projection_matrix_dataref, glm::value_ptr(projection_matrix), 0, projection_matrix.length() * projection_matrix.length());

		inverse_projection_matrix = glm::inverse(projection_matrix);
		inverse_modelview_matrix = glm::inverse(modelview_matrix);

		cloud_map_scale = XPLMGetDataf(cloud_map_scale_dataref);

		base_noise_scale = XPLMGetDataf(base_noise_scale_dataref);
		detail_noise_scale = XPLMGetDataf(detail_noise_scale_dataref);

		blue_noise_scale = XPLMGetDataf(blue_noise_scale_dataref);

		for (int layer_index = 0; layer_index < CLOUD_LAYER_COUNT; layer_index++) cloud_types[layer_index] = XPLMGetDatai(cloud_type_datarefs[layer_index]);

		for (int layer_index = 0; layer_index < CLOUD_LAYER_COUNT; layer_index++)
		{
			cloud_bases[layer_index] = XPLMGetDataf(cloud_base_datarefs[layer_index]);

			float new_height;

			if (cloud_types[layer_index] == 1) new_height = 250.0;
			else new_height = glm::max((XPLMGetDataf(cloud_top_datarefs[layer_index]) - cloud_bases[layer_index]) / 0.75f, 2500.0f);

			cloud_tops[layer_index] = cloud_bases[layer_index] + new_height;
		}

		for (int type_index = 0; type_index < CLOUD_TYPE_COUNT; type_index++)
		{
			cloud_coverages[type_index] = XPLMGetDataf(cloud_coverage_datarefs[type_index]);
			cloud_densities[type_index] = XPLMGetDataf(cloud_density_datarefs[type_index]);
		}

		for (int type_index = 0; type_index < CLOUD_TYPE_COUNT; type_index++)
		{
			XPLMGetDatavf(base_noise_ratio_datarefs[type_index], glm::value_ptr(base_noise_ratios[type_index]), 0, base_noise_ratios[type_index].length());
			XPLMGetDatavf(detail_noise_ratio_datarefs[type_index], glm::value_ptr(detail_noise_ratios[type_index]), 0, detail_noise_ratios[type_index].length());
		}

		float wind_altitudes[WIND_LAYER_COUNT];
		glm::vec3 wind_vectors[WIND_LAYER_COUNT];

		for (int layer_index = 0; layer_index < WIND_LAYER_COUNT; layer_index++)
		{
			wind_altitudes[layer_index] = XPLMGetDataf(wind_altitude_datarefs[layer_index]);

			float wind_heading = glm::radians(XPLMGetDataf(wind_direction_datarefs[layer_index]));
			wind_vectors[layer_index] = glm::vec3(glm::sin(wind_heading), 0.0f, -1.0f * glm::cos(wind_heading)) * XPLMGetDataf(wind_speed_datarefs[layer_index]) * MPS_PER_KNOTS;
		}

		previous_zulu_time = current_zulu_time;
		current_zulu_time = XPLMGetDataf(zulu_time_dataref);

		float time_difference = current_zulu_time - previous_zulu_time;

		if (time_difference < -3601.0) time_difference = fabs(time_difference);

		if (time_difference > 5.0 && time_difference < 3600.0 || time_difference < -5.0)
		{
			time_accelerated = 1;
		}
		else
		{
			time_accelerated = 0;
		}
			

		if (!time_accelerated)
		{
			if(combined_count < sample_count-1)
			{
				if (time_difference > 3600.0) time_difference = time_difference_average;
				combined_count++;
				time_difference_combined[combined_count] = time_difference;
				for (int i = 0; i < sample_count; i++)time_difference_average += time_difference_combined[i];
				time_difference_average /= sample_count;
			}
			else
			{
				combined_count = 0;
			}
		}

		XPLMSetDataf(matts_fdebug_dataref, time_difference_average);
		XPLMSetDatai(matts_idebug_dataref, combined_count);

		for (int cloud_layer_index = 0; cloud_layer_index < CLOUD_LAYER_COUNT; cloud_layer_index++)
		{
			for (int wind_layer_index = 0; wind_layer_index < WIND_LAYER_COUNT; wind_layer_index++) wind_offsets[cloud_layer_index] += wind_vectors[wind_layer_index] * glm::clamp(glm::pow(glm::abs(cloud_bases[cloud_layer_index] - wind_altitudes[wind_layer_index]) * 0.001f, 2.0f), 0.0f, 1.0f) * time_difference;

			if (!time_accelerated)
			{
				wind_offsets[cloud_layer_index].y += 0.05 * time_difference_average;
			}
			else
			{
				wind_offsets[cloud_layer_index].y += 0.05 * time_difference;
			}
		}

		XPLMDataRef fade_start_distance_dataref = XPLMFindDataRef("sim/private/stats/skyc/fog/near_fog_cld");
		XPLMDataRef fade_end_distance_dataref = XPLMFindDataRef("sim/private/stats/skyc/fog/far_fog_cld");

		fade_start_distance = XPLMGetDataf(fade_start_distance_dataref);
		fade_end_distance = XPLMGetDataf(fade_end_distance_dataref);

		float sun_pitch = glm::radians(XPLMGetDataf(sun_pitch_dataref));
		float sun_heading = glm::radians(XPLMGetDataf(sun_heading_dataref));
		float moon_pitch = glm::radians(XPLMGetDataf(moon_pitch_dataref));
		float moon_heading = glm::radians(XPLMGetDataf(moon_heading_dataref));

		sun_direction = glm::vec3(glm::cos(sun_pitch) * glm::sin(sun_heading), glm::sin(sun_pitch), -1.0f * glm::cos(sun_pitch) * glm::cos(sun_heading));
		moon_direction = glm::vec3(glm::cos(moon_pitch) * glm::sin(moon_heading), glm::sin(moon_pitch), -1.0f * glm::cos(moon_pitch) * glm::cos(moon_heading));

		sun_tint_red_dataref = XPLMFindDataRef("sim/private/stats/skyc/sun_dir_r");
		sun_tint_green_dataref = XPLMFindDataRef("sim/private/stats/skyc/sun_dir_g");
		sun_tint_blue_dataref = XPLMFindDataRef("sim/private/stats/skyc/sun_dir_b");

		sun_tint = glm::vec3(XPLMGetDataf(sun_tint_red_dataref), XPLMGetDataf(sun_tint_green_dataref), XPLMGetDataf(sun_tint_blue_dataref));
		sun_gain = XPLMGetDataf(sun_gain_dataref);
		moon_gain = XPLMGetDataf(moon_gain_dataref);
		moon_glow = XPLMGetDataf(moon_glow_dataref);

		ambient_tint = glm::vec3(XPLMGetDataf(ambient_tint_red_dataref), XPLMGetDataf(ambient_tint_green_dataref), XPLMGetDataf(ambient_tint_blue_dataref));
		ambient_gain = XPLMGetDataf(ambient_gain_dataref);

		backward_mie_scattering = XPLMGetDataf(backward_mie_scattering_dataref);
		forward_mie_scattering = XPLMGetDataf(forward_mie_scattering_dataref);

		XPLMGetDatavf(atmosphere_bottom_tint_dataref, glm::value_ptr(atmosphere_bottom_tint), 0, atmosphere_bottom_tint.length());
		XPLMGetDatavf(atmosphere_top_tint_dataref, glm::value_ptr(atmosphere_top_tint), 0, atmosphere_top_tint.length());

		atmospheric_blending = XPLMGetDataf(atmospheric_blending_dataref);
		bicubic_sampling = XPLMGetDatai(bicubic_sampling_dataref);

		day_numeric = XPLMGetDatai(day_numeric_dataref);
		month_numeric = XPLMGetDatai(month_numeric_dataref);
		if (check_day_numeric != day_numeric || check_month_numeric != month_numeric)
		{
		check_day_numeric = day_numeric;
		check_month_numeric = month_numeric;
		moon_phase = MoonPhase::GetPhase(month_numeric, day_numeric, year_numeric);
		XPLMSetDataf(moon_phase_dataref, moon_phase);

		moon_phase_percent = MoonPhase::GetPercentage(moon_phase);
		XPLMSetDataf(moon_glow_dataref, moon_phase_percent);
		}

	}
}