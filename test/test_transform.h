#undef UFBXT_TEST_GROUP
#define UFBXT_TEST_GROUP "transform"

UFBXT_FILE_TEST(maya_pivots)
#if UFBXT_IMPL
{
	ufbx_node *node = ufbx_find_node(scene, "pCube1");
	ufbxt_assert(node);

	ufbx_vec3 origin_ref = { (ufbx_real)0.7211236250, (ufbx_real)1.8317762500, (ufbx_real)-0.6038020000 };
	ufbxt_assert_close_vec3(err, node->local_transform.translation, origin_ref);
}
#endif

#if UFBXT_IMPL
static void ufbxt_check_rotation_order(ufbx_scene *scene, const char *name, ufbx_rotation_order order)
{
	ufbx_node *node = ufbx_find_node(scene, name);
	ufbxt_assert(node);
	ufbx_prop *prop = ufbx_find_prop(&node->props, "RotationOrder");
	ufbxt_assert(prop);
	ufbxt_assert((ufbx_rotation_order)prop->value_int == order);
}
#endif

UFBXT_FILE_TEST(maya_rotation_order)
#if UFBXT_IMPL
{
	ufbxt_check_rotation_order(scene, "XYZ", UFBX_ROTATION_ORDER_XYZ);
	ufbxt_check_rotation_order(scene, "XZY", UFBX_ROTATION_ORDER_XZY);
	ufbxt_check_rotation_order(scene, "YZX", UFBX_ROTATION_ORDER_YZX);
	ufbxt_check_rotation_order(scene, "YXZ", UFBX_ROTATION_ORDER_YXZ);
	ufbxt_check_rotation_order(scene, "ZXY", UFBX_ROTATION_ORDER_ZXY);
	ufbxt_check_rotation_order(scene, "ZYX", UFBX_ROTATION_ORDER_ZYX);
}
#endif

UFBXT_FILE_TEST(maya_post_rotate_order)
#if UFBXT_IMPL
{
	ufbxt_check_rotation_order(scene, "pCube1", UFBX_ROTATION_ORDER_XYZ);
	ufbxt_check_rotation_order(scene, "pCube2", UFBX_ROTATION_ORDER_ZYX);
}
#endif

UFBXT_FILE_TEST(synthetic_pre_post_rotate)
#if UFBXT_IMPL
{
	ufbxt_check_rotation_order(scene, "pCube1", UFBX_ROTATION_ORDER_XYZ);
	ufbxt_check_rotation_order(scene, "pCube2", UFBX_ROTATION_ORDER_ZYX);
}
#endif

UFBXT_FILE_TEST(maya_parented_cubes)
#if UFBXT_IMPL
{
}
#endif

UFBXT_FILE_TEST_FLAGS(synthetic_geometric_squish, UFBXT_FILE_TEST_FLAG_OPT_HANDLING_IGNORE_NORMALS_IN_DIFF)
#if UFBXT_IMPL
{
	ufbx_node *node = ufbx_find_node(scene, "pSphere1");
	ufbxt_assert(node);
	ufbxt_assert_close_real(err, node->geometry_transform.scale.y, 0.01f);
}
#endif

UFBXT_FILE_TEST_FLAGS(synthetic_geometric_transform, UFBXT_FILE_TEST_FLAG_OPT_HANDLING_IGNORE_NORMALS_IN_DIFF)
#if UFBXT_IMPL
{
	ufbx_node *node = ufbx_find_node(scene, "Parent");
	ufbxt_assert(node);
	ufbx_vec3 euler = ufbx_quat_to_euler(node->geometry_transform.rotation, UFBX_ROTATION_ORDER_XYZ);
	ufbxt_assert_close_real(err, node->geometry_transform.translation.x, -0.5f);
	ufbxt_assert_close_real(err, node->geometry_transform.translation.y, -1.0f);
	ufbxt_assert_close_real(err, node->geometry_transform.translation.z, -1.5f);
	ufbxt_assert_close_real(err, euler.x, 20.0f);
	ufbxt_assert_close_real(err, euler.y, 40.0f);
	ufbxt_assert_close_real(err, euler.z, 60.0f);
	ufbxt_assert_close_real(err, node->geometry_transform.scale.x, 2.0f);
	ufbxt_assert_close_real(err, node->geometry_transform.scale.y, 3.0f);
	ufbxt_assert_close_real(err, node->geometry_transform.scale.z, 4.0f);
}
#endif

UFBXT_FILE_TEST(maya_cube_hidden)
#if UFBXT_IMPL
{
	ufbx_node *node = ufbx_find_node(scene, "pCube1");
	ufbxt_assert(node);
	ufbxt_assert(!node->visible);
}
#endif

UFBXT_TEST(root_transform)
#if UFBXT_IMPL
{

	ufbxt_diff_error err = { 0 };

	char path[512];
	ufbxt_file_iterator iter = { "maya_cube" };
	while (ufbxt_next_file(&iter, path, sizeof(path))) {
		ufbx_load_opts opts = { 0 };

		ufbx_vec3 euler = { { 90.0f, 0.0f, 0.0f } };

		opts.use_root_transform = true;
		opts.root_transform.translation.x = -1.0f;
		opts.root_transform.translation.y = -2.0f;
		opts.root_transform.translation.z = -3.0f;
		opts.root_transform.rotation = ufbx_euler_to_quat(euler, UFBX_ROTATION_ORDER_XYZ);
		opts.root_transform.scale.x = 2.0f;
		opts.root_transform.scale.y = 3.0f;
		opts.root_transform.scale.z = 4.0f;

		ufbx_scene *scene = ufbx_load_file(path, &opts, NULL);
		ufbxt_assert(scene);

		ufbxt_check_scene(scene);

		ufbxt_assert_close_vec3(&err, scene->root_node->local_transform.translation, opts.root_transform.translation);
		ufbxt_assert_close_quat(&err, scene->root_node->local_transform.rotation, opts.root_transform.rotation);
		ufbxt_assert_close_vec3(&err, scene->root_node->local_transform.scale, opts.root_transform.scale);

		ufbx_transform eval = ufbx_evaluate_transform(scene->anim, scene->root_node, 0.1);
		ufbxt_assert_close_vec3(&err, eval.translation, opts.root_transform.translation);
		ufbxt_assert_close_quat(&err, eval.rotation, opts.root_transform.rotation);
		ufbxt_assert_close_vec3(&err, eval.scale, opts.root_transform.scale);

		ufbx_scene *state = ufbx_evaluate_scene(scene, scene->anim, 0.1, NULL, NULL);
		ufbxt_assert(state);

		ufbxt_assert_close_vec3(&err, state->root_node->local_transform.translation, opts.root_transform.translation);
		ufbxt_assert_close_quat(&err, state->root_node->local_transform.rotation, opts.root_transform.rotation);
		ufbxt_assert_close_vec3(&err, state->root_node->local_transform.scale, opts.root_transform.scale);
		
		ufbx_free_scene(state);
		ufbx_free_scene(scene);
	}

	ufbxt_assert(iter.num_found >= 8);

	ufbxt_logf(".. Absolute diff: avg %.3g, max %.3g (%zu tests)", err.sum / (ufbx_real)err.num, err.max, err.num);
}
#endif

UFBXT_TEST(blender_axes)
#if UFBXT_IMPL
{
	char path[512], name[512];

	static const char *axis_names[] = {
		"px", "nx", "py", "ny", "pz", "nz",
	};

	for (uint32_t fwd_ix = 0; fwd_ix < 6; fwd_ix++) {
		for (uint32_t up_ix = 0; up_ix < 6; up_ix++) {

			// Don't allow collinear axes
			if ((fwd_ix >> 1) == (up_ix >> 1)) continue;

			ufbx_coordinate_axis axis_fwd = (ufbx_coordinate_axis)fwd_ix;
			ufbx_coordinate_axis axis_up = (ufbx_coordinate_axis)up_ix;

			snprintf(name, sizeof(name), "blender_axes/axes_%s%s", axis_names[fwd_ix], axis_names[up_ix]);

			ufbxt_file_iterator iter = { name };
			while (ufbxt_next_file(&iter, path, sizeof(path))) {
				ufbxt_diff_error err = { 0 };

				// Load normally and check axes
				{
					ufbx_scene *scene = ufbx_load_file(path, NULL, NULL);
					ufbxt_assert(scene);
					ufbxt_check_scene(scene);

					ufbx_coordinate_axis axis_front = axis_fwd ^ 1;

					ufbxt_assert(scene->settings.axes.front == axis_front);
					ufbxt_assert(scene->settings.axes.up == axis_up);

					ufbx_free_scene(scene);
				}

				// Axis conversion
				for (int mode = 0; mode < 4; mode++) {
					ufbxt_hintf("mode = %d", mode);
					ufbx_load_opts opts = { 0 };

					bool transform_root = (mode % 2) != 0;
					bool use_adjust = (mode / 2) != 0;

					opts.target_axes = ufbx_axes_right_handed_z_up;
					opts.target_unit_meters = 1.0f;

					if (transform_root) {
						opts.use_root_transform = true;
						opts.root_transform = ufbx_identity_transform;
						opts.root_transform.translation.z = 1.0f;
					}

					if (use_adjust) {
						opts.space_conversion = UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS;
					} else {
						opts.space_conversion = UFBX_SPACE_CONVERSION_TRANSFORM_ROOT;
					}

					ufbx_scene *scene = ufbx_load_file(path, &opts, NULL);
					ufbxt_assert(scene);
					ufbxt_check_scene(scene);

					ufbx_node *plane = ufbx_find_node(scene, "Plane");
					ufbxt_assert(plane && plane->mesh);
					ufbx_mesh *mesh = plane->mesh;

					ufbxt_assert(mesh->num_faces == 1);
					ufbx_face face = mesh->faces.data[0];
					ufbxt_assert(face.num_indices == 3);

					if (use_adjust && !transform_root) {
						ufbx_node *root = scene->root_node;

						ufbx_vec3 identity_scale = { 1.0f, 1.0f, 1.0f };
						ufbxt_assert_close_quat(&err, root->local_transform.rotation, ufbx_identity_quat);
						ufbxt_assert_close_vec3(&err, root->local_transform.scale, identity_scale);
					}

					for (uint32_t i = 0; i < face.num_indices; i++) {
						ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, face.index_begin + i);
						ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, face.index_begin + i);

						pos = ufbx_transform_position(&plane->geometry_to_world, pos);

						ufbx_vec3 ref;
						if (uv.x < 0.5f && uv.y < 0.5f) {
							ref.x = 1.0f;
							ref.y = 1.0f;
							ref.z = 3.0f;
						} else if (uv.x > 0.5f && uv.y < 0.5f) {
							ref.x = 2.0f;
							ref.y = 2.0f;
							ref.z = 3.0f;
						} else if (uv.x < 0.5f && uv.y > 0.5f) {
							ref.x = 1.0f;
							ref.y = 2.0f;
							ref.z = 4.0f;
						} else {
							ufbxt_assert(0 && "Shouldn't exist");
						}

						if (transform_root) {
							ref.z += 1.0f;
						}

						ufbxt_assert_close_vec3(&err, pos, ref);
					}

					ufbx_free_scene(scene);
				}

				ufbxt_logf(".. Absolute diff: avg %.3g, max %.3g (%zu tests)", err.sum / (ufbx_real)err.num, err.max, err.num);
			}
		}
	}
}
#endif

#if UFBXT_IMPL
static ufbx_load_opts ufbxt_scale_to_cm_opts()
{
	ufbx_load_opts opts = { 0 };
	opts.target_unit_meters = 0.01f;
	return opts;
}
static ufbx_load_opts ufbxt_scale_to_cm_adjust_opts()
{
	ufbx_load_opts opts = { 0 };
	opts.target_unit_meters = 0.01f;
	opts.space_conversion = UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS;
	return opts;
}
static ufbx_load_opts ufbxt_scale_to_cm_helper_opts()
{
	ufbx_load_opts opts = { 0 };
	opts.target_unit_meters = 0.01f;
	opts.inherit_mode_handling = UFBX_INHERIT_MODE_HANDLING_HELPER_NODES;
	return opts;
}
static ufbx_load_opts ufbxt_scale_to_cm_adjust_helper_opts()
{
	ufbx_load_opts opts = { 0 };
	opts.target_unit_meters = 0.01f;
	opts.space_conversion = UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS;
	opts.inherit_mode_handling = UFBX_INHERIT_MODE_HANDLING_HELPER_NODES;
	return opts;
}
static ufbx_load_opts ufbxt_scale_to_cm_compensate_opts()
{
	ufbx_load_opts opts = { 0 };
	opts.target_unit_meters = 0.01f;
	opts.inherit_mode_handling = UFBX_INHERIT_MODE_HANDLING_COMPENSATE;
	return opts;
}
static ufbx_load_opts ufbxt_scale_to_cm_adjust_compensate_opts()
{
	ufbx_load_opts opts = { 0 };
	opts.target_unit_meters = 0.01f;
	opts.space_conversion = UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS;
	opts.inherit_mode_handling = UFBX_INHERIT_MODE_HANDLING_COMPENSATE;
	return opts;
}
#endif

UFBXT_FILE_TEST_OPTS(maya_scale_no_inherit, ufbxt_scale_to_cm_opts)
#if UFBXT_IMPL
{
	{
		ufbx_node *node = ufbx_find_node(scene, "joint1");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);
		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 0.03f, 0.03f, 0.03f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 3.0f, 3.0f, 3.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 0.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint2");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_IGNORE_PARENT_SCALE);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);
		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 100.0f, 100.0f, 100.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 6.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_IGNORE_PARENT_SCALE);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);
		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 0.01f, 0.01f, 0.01f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 206.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint4");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_IGNORE_PARENT_SCALE);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);
		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 0.015f, 0.025f, 0.035f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.5f, 2.5f, 3.5f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 208.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, node, 1.0);
			ufbxt_assert_close_real(err, transform.scale.x * 100.0f, 0.3f);
			ufbxt_assert_close_real(err, transform.scale.y * 100.0f, 0.6f);
			ufbxt_assert_close_real(err, transform.scale.z * 100.0f, 0.9f);
		}

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, node, 0.5);
			ufbxt_assert_close_real(err, transform.scale.x * 100.0f, 0.67281f);
			ufbxt_assert_close_real(err, transform.scale.y * 100.0f, 0.81304f);
			ufbxt_assert_close_real(err, transform.scale.z * 100.0f, 0.95326f);
		}
	}
}
#endif

UFBXT_FILE_TEST_OPTS_ALT(maya_scale_no_inherit_adjust, maya_scale_no_inherit, ufbxt_scale_to_cm_adjust_opts)
#if UFBXT_IMPL
{
	{
		ufbx_node *node = ufbx_find_node(scene, "joint1");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_IGNORE_PARENT_SCALE);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);
		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 3.0f, 3.0f, 3.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 3.0f, 3.0f, 3.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 0.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint2");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_IGNORE_PARENT_SCALE);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);
		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 100.0f, 100.0f, 100.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 100.0f, 100.0f, 100.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 6.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_IGNORE_PARENT_SCALE);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);
		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 206.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint4");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_IGNORE_PARENT_SCALE);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);
		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 1.5f, 2.5f, 3.5f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.5f, 2.5f, 3.5f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 208.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, node, 1.0);
			ufbxt_assert_close_vec3_xyz(err, transform.scale, 0.3f, 0.6f, 0.9f);
		}

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, node, 0.5);
			ufbxt_assert_close_vec3_xyz(err, transform.scale, 0.67281f, 0.81304f, 0.95326f);
		}
	}
}
#endif

UFBXT_FILE_TEST_OPTS_ALT(maya_scale_no_inherit_helper, maya_scale_no_inherit, ufbxt_scale_to_cm_helper_opts)
#if UFBXT_IMPL
{
	{
		ufbx_node *node = ufbx_find_node(scene, "joint1");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_node *helper = node->scale_helper;
		ufbxt_assert(helper);
		ufbxt_assert(helper->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&helper->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, helper->local_transform.scale, 0.03f, 0.03f, 0.03f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 3.0f, 3.0f, 3.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 0.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint2");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbxt_assert(!node->scale_helper);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 100.0f, 100.0f, 100.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 6.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_node *helper = node->scale_helper;
		ufbxt_assert(helper);
		ufbxt_assert(helper->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&helper->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, helper->local_transform.scale, 0.01f, 0.01f, 0.01f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 206.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint4");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbxt_assert(!node->scale_helper);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 0.015f, 0.025f, 0.035f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.5f, 2.5f, 3.5f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 208.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbx_node *helper = node->scale_helper;
		ufbxt_assert(helper);

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, helper, 1.0);
			ufbxt_assert_close_real(err, transform.scale.x * 100.0f, 0.3f);
			ufbxt_assert_close_real(err, transform.scale.y * 100.0f, 0.6f);
			ufbxt_assert_close_real(err, transform.scale.z * 100.0f, 0.9f);
		}

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, helper, 0.5);
			ufbxt_assert_close_real(err, transform.scale.x * 100.0f, 0.67281f);
			ufbxt_assert_close_real(err, transform.scale.y * 100.0f, 0.81304f);
			ufbxt_assert_close_real(err, transform.scale.z * 100.0f, 0.95326f);
		}
	}
}
#endif

UFBXT_FILE_TEST_OPTS_ALT(maya_scale_no_inherit_adjust_helper, maya_scale_no_inherit, ufbxt_scale_to_cm_adjust_helper_opts)
#if UFBXT_IMPL
{
	{
		ufbx_node *node = ufbx_find_node(scene, "joint1");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_node *helper = node->scale_helper;
		ufbxt_assert(helper);
		ufbxt_assert(helper->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&helper->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 100.0f, 100.0f, 100.0f);
		ufbxt_assert_close_vec3_xyz(err, helper->local_transform.scale, 0.03f, 0.03f, 0.03f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 3.0f, 3.0f, 3.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 0.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint2");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbxt_assert(!node->scale_helper);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 100.0f, 100.0f, 100.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 6.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_node *helper = node->scale_helper;
		ufbxt_assert(helper);
		ufbxt_assert(helper->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&helper->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, helper->local_transform.scale, 0.01f, 0.01f, 0.01f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 206.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint4");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbxt_assert(!node->scale_helper);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 0.015f, 0.025f, 0.035f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.5f, 2.5f, 3.5f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 208.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbx_node *helper = node->scale_helper;
		ufbxt_assert(helper);

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, helper, 1.0);
			ufbxt_assert_close_real(err, transform.scale.x * 100.0f, 0.3f);
			ufbxt_assert_close_real(err, transform.scale.y * 100.0f, 0.6f);
			ufbxt_assert_close_real(err, transform.scale.z * 100.0f, 0.9f);
		}

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, helper, 0.5);
			ufbxt_assert_close_real(err, transform.scale.x * 100.0f, 0.67281f);
			ufbxt_assert_close_real(err, transform.scale.y * 100.0f, 0.81304f);
			ufbxt_assert_close_real(err, transform.scale.z * 100.0f, 0.95326f);
		}
	}
}
#endif

UFBXT_FILE_TEST_OPTS_ALT(maya_scale_no_inherit_compensate, maya_scale_no_inherit, ufbxt_scale_to_cm_compensate_opts)
#if UFBXT_IMPL
{
	{
		ufbx_node *node = ufbx_find_node(scene, "joint1");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbxt_assert(!node->scale_helper);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 0.03f, 0.03f, 0.03f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 3.0f, 3.0f, 3.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 0.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint2");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbxt_assert(!node->scale_helper);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 33.33333f, 33.33333f, 33.33333f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 100.0f, 100.0f, 100.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 6.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_node *helper = node->scale_helper;
		ufbxt_assert(helper);
		ufbxt_assert(helper->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&helper->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, helper->local_transform.scale, 0.01f, 0.01f, 0.01f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 206.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint4");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbxt_assert(!node->scale_helper);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 0.015f, 0.025f, 0.035f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.5f, 2.5f, 3.5f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 208.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbx_node *helper = node->scale_helper;
		ufbxt_assert(helper);

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, helper, 1.0);
			ufbxt_assert_close_real(err, transform.scale.x * 100.0f, 0.3f);
			ufbxt_assert_close_real(err, transform.scale.y * 100.0f, 0.6f);
			ufbxt_assert_close_real(err, transform.scale.z * 100.0f, 0.9f);
		}

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, helper, 0.5);
			ufbxt_assert_close_real(err, transform.scale.x * 100.0f, 0.67281f);
			ufbxt_assert_close_real(err, transform.scale.y * 100.0f, 0.81304f);
			ufbxt_assert_close_real(err, transform.scale.z * 100.0f, 0.95326f);
		}
	}
}
#endif

UFBXT_FILE_TEST_OPTS_ALT(maya_scale_no_inherit_adjust_compensate, maya_scale_no_inherit, ufbxt_scale_to_cm_adjust_compensate_opts)
#if UFBXT_IMPL
{
	{
		ufbx_node *node = ufbx_find_node(scene, "joint1");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbxt_assert(!node->scale_helper);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 3.0f, 3.0f, 3.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 3.0f, 3.0f, 3.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 0.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint2");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbxt_assert(!node->scale_helper);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 33.33333f, 33.33333f, 33.33333f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 100.0f, 100.0f, 100.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 0.0f, 6.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_node *helper = node->scale_helper;
		ufbxt_assert(helper);
		ufbxt_assert(helper->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&helper->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, helper->local_transform.scale, 0.01f, 0.01f, 0.01f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.0f, 1.0f, 1.0f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 206.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint4");
		ufbxt_assert(node);
		ufbxt_assert(node->inherit_mode == UFBX_INHERIT_MODE_NORMAL);
		ufbxt_assert(!node->scale_helper);
		ufbx_transform world_transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3_xyz(err, node->local_transform.scale, 0.015f, 0.025f, 0.035f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.scale, 1.5f, 2.5f, 3.5f);
		ufbxt_assert_close_vec3_xyz(err, world_transform.translation, 200.0f, 208.0f, 0.0f);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbx_node *helper = node->scale_helper;
		ufbxt_assert(helper);

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, helper, 1.0);
			ufbxt_assert_close_real(err, transform.scale.x * 100.0f, 0.3f);
			ufbxt_assert_close_real(err, transform.scale.y * 100.0f, 0.6f);
			ufbxt_assert_close_real(err, transform.scale.z * 100.0f, 0.9f);
		}

		{
			ufbx_transform transform = ufbx_evaluate_transform(scene->anim, helper, 0.5);
			ufbxt_assert_close_real(err, transform.scale.x * 100.0f, 0.67281f);
			ufbxt_assert_close_real(err, transform.scale.y * 100.0f, 0.81304f);
			ufbxt_assert_close_real(err, transform.scale.z * 100.0f, 0.95326f);
		}
	}
}
#endif

UFBXT_FILE_TEST(synthetic_node_dag)
#if UFBXT_IMPL
{
	ufbx_node *root = scene->root_node;
	ufbx_node *a = ufbx_find_node(scene, "A");
	ufbx_node *b = ufbx_find_node(scene, "B");
	ufbx_node *c = ufbx_find_node(scene, "C");
	ufbx_node *d = ufbx_find_node(scene, "D");

	ufbxt_assert(root && a && b && c && d);
	ufbxt_assert(root->children.count == 1);
	ufbxt_assert(root->children.data[0] == a);

	ufbxt_assert(a->parent == root);
	ufbxt_assert(a->children.count == 1);
	ufbxt_assert(a->children.data[0] == b);

	ufbxt_assert(b->parent == a);
	ufbxt_assert(b->children.count == 1);
	ufbxt_assert(b->children.data[0] == c);

	ufbxt_assert(c->parent == b);
	ufbxt_assert(c->children.count == 1);
	ufbxt_assert(c->children.data[0] == d);

	ufbxt_assert(d->parent == c);
	ufbxt_assert(d->children.count == 0);
}
#endif

UFBXT_FILE_TEST_FLAGS(synthetic_node_cycle_fail, UFBXT_FILE_TEST_FLAG_ALLOW_ERROR)
#if UFBXT_IMPL
{
	ufbxt_assert(!scene);
}
#endif

UFBXT_FILE_TEST(maya_static_no_inherit_scale)
#if UFBXT_IMPL
{
	{
		ufbx_node *node = ufbx_find_node(scene, "joint1");
		ufbxt_assert(node);

		ufbx_vec3 ref_translation = { 0.0f, 0.0f, 0.0f };
		ufbx_vec3 ref_scale = { 2.0f, 2.0f, 2.0f };
		ufbx_transform transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3(err, ref_translation, transform.translation);
		ufbxt_assert_close_vec3(err, ref_scale, transform.scale);
		ufbxt_assert_close_vec3(err, ref_scale, node->local_transform.scale);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint2");
		ufbxt_assert(node);

		ufbx_vec3 ref_translation = { 0.0f, 8.0f, 0.0f };
		ufbx_vec3 ref_scale = { 1.0f, 2.0f, 2.0f };
		ufbx_transform transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3(err, ref_translation, transform.translation);
		ufbxt_assert_close_vec3(err, ref_scale, transform.scale);
		ufbxt_assert_close_vec3(err, ref_scale, node->local_transform.scale);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbxt_assert(node);

		ufbx_vec3 ref_translation = { 0.0f, 12.0f, 0.0f };
		ufbx_vec3 ref_scale = { 1.0f, 1.0f, 1.0f };
		ufbx_transform transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3(err, ref_translation, transform.translation);
		ufbxt_assert_close_vec3(err, ref_scale, transform.scale);
		ufbxt_assert_close_vec3(err, ref_scale, node->local_transform.scale);
	}
}
#endif

#if UFBXT_IMPL
static ufbx_load_opts ufbxt_scale_helper_opts()
{
	ufbx_load_opts opts = { 0 };
	opts.inherit_mode_handling = UFBX_INHERIT_MODE_HANDLING_HELPER_NODES;
	return opts;
}
static ufbx_load_opts ufbxt_scale_compensate_opts()
{
	ufbx_load_opts opts = { 0 };
	opts.inherit_mode_handling = UFBX_INHERIT_MODE_HANDLING_COMPENSATE;
	return opts;
}
#endif

UFBXT_FILE_TEST_OPTS_ALT_FLAGS(maya_static_no_inherit_scale_helper, maya_static_no_inherit_scale, ufbxt_scale_helper_opts, UFBXT_FILE_TEST_FLAG_FUZZ_ALWAYS|UFBXT_FILE_TEST_FLAG_FUZZ_OPTS)
#if UFBXT_IMPL
{
	ufbx_vec3 ref_scale_one = { 1.0f, 1.0f, 1.0f };

	{
		ufbx_node *node = ufbx_find_node(scene, "joint1");
		ufbxt_assert(node);
		ufbxt_assert(node->scale_helper);

		ufbx_vec3 ref_translation = { 0.0f, 0.0f, 0.0f };
		ufbx_vec3 ref_scale = { 2.0f, 2.0f, 2.0f };
		ufbx_transform transform = ufbx_matrix_to_transform(&node->scale_helper->node_to_world);

		ufbxt_assert_close_vec3(err, ref_translation, transform.translation);
		ufbxt_assert_close_vec3(err, ref_scale, transform.scale);
		ufbxt_assert_close_vec3(err, ref_scale, node->scale_helper->local_transform.scale);
		ufbxt_assert_close_vec3(err, ref_scale_one, node->local_transform.scale);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint2");
		ufbxt_assert(node);
		ufbxt_assert(node->scale_helper);

		ufbx_vec3 ref_translation = { 0.0f, 8.0f, 0.0f };
		ufbx_vec3 ref_scale = { 1.0f, 2.0f, 2.0f };
		ufbx_transform transform = ufbx_matrix_to_transform(&node->scale_helper->node_to_world);

		ufbxt_assert_close_vec3(err, ref_translation, transform.translation);
		ufbxt_assert_close_vec3(err, ref_scale, transform.scale);
		ufbxt_assert_close_vec3(err, ref_scale, node->scale_helper->local_transform.scale);
		ufbxt_assert_close_vec3(err, ref_scale_one, node->local_transform.scale);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbxt_assert(node);
		ufbxt_assert(!node->scale_helper);

		ufbx_vec3 ref_translation = { 0.0f, 12.0f, 0.0f };
		ufbx_vec3 ref_scale = { 1.0f, 1.0f, 1.0f };
		ufbx_transform transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3(err, ref_translation, transform.translation);
		ufbxt_assert_close_vec3(err, ref_scale, transform.scale);
	}
}
#endif

UFBXT_FILE_TEST_OPTS_ALT_FLAGS(maya_static_no_inherit_scale_compensate, maya_static_no_inherit_scale, ufbxt_scale_compensate_opts, UFBXT_FILE_TEST_FLAG_FUZZ_ALWAYS|UFBXT_FILE_TEST_FLAG_FUZZ_OPTS)
#if UFBXT_IMPL
{
	ufbx_vec3 ref_scale_one = { 1.0f, 1.0f, 1.0f };

	{
		ufbx_node *node = ufbx_find_node(scene, "joint1");
		ufbxt_assert(node);

		ufbx_vec3 ref_translation = { 0.0f, 0.0f, 0.0f };
		ufbx_vec3 ref_scale = { 2.0f, 2.0f, 2.0f };
		ufbx_transform transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3(err, ref_translation, transform.translation);
		ufbxt_assert_close_vec3(err, ref_scale, transform.scale);
		ufbxt_assert_close_vec3(err, ref_scale, node->local_transform.scale);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint2");
		ufbxt_assert(node);
		ufbxt_assert(node->scale_helper);
		ufbxt_assert_close_real(err, node->adjust_post_scale, 0.5f);

		ufbx_vec3 ref_translation = { 0.0f, 8.0f, 0.0f };
		ufbx_vec3 ref_scale = { 1.0f, 2.0f, 2.0f };
		ufbx_vec3 local_scale = { 0.5f, 0.5f, 0.5f };
		ufbx_transform transform = ufbx_matrix_to_transform(&node->scale_helper->node_to_world);

		ufbxt_assert_close_vec3(err, ref_translation, transform.translation);
		ufbxt_assert_close_vec3(err, ref_scale, transform.scale);
		ufbxt_assert_close_vec3(err, ref_scale, node->scale_helper->local_transform.scale);
		ufbxt_assert_close_vec3(err, local_scale, node->local_transform.scale);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "joint3");
		ufbxt_assert(node);
		ufbxt_assert(!node->scale_helper);

		ufbx_vec3 ref_translation = { 0.0f, 12.0f, 0.0f };
		ufbx_vec3 ref_scale = { 1.0f, 1.0f, 1.0f };
		ufbx_transform transform = ufbx_matrix_to_transform(&node->node_to_world);

		ufbxt_assert_close_vec3(err, ref_translation, transform.translation);
		ufbxt_assert_close_vec3(err, ref_scale, transform.scale);
	}
}
#endif

