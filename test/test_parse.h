
UFBXT_TEST(thread_safety)
#if UFBXT_IMPL
{
	if (!g_allow_non_thread_safe) {
		ufbxt_assert(ufbx_is_thread_safe());
	}
}
#endif

UFBXT_FILE_TEST(maya_leading_comma)
#if UFBXT_IMPL
{
	ufbxt_assert(!strcmp(scene->metadata.creator.data, "FBX SDK/FBX Plugins version 2019.2"));
}
#endif

UFBXT_FILE_TEST(maya_zero_end)
#if UFBXT_IMPL
{
	ufbxt_assert(!strcmp(scene->metadata.creator.data, "FBX SDK/FBX Plugins version 2019.2"));
}
#endif

UFBXT_TEST(error_format_long)
#if UFBXT_IMPL
{
	char data[] = "Bad FBX";
	ufbx_error error;
	ufbx_scene *scene = ufbx_load_memory(data, sizeof(data), NULL, &error);
	ufbxt_assert(!scene);

	char error_buf[512];
	size_t length = ufbx_format_error(error_buf, sizeof(error_buf), &error);
	ufbxt_assert(strlen(error_buf) == length);

	size_t num_lines = 0;
	for (size_t i = 0; i < length; i++) {
		if (error_buf[i] == '\n') num_lines++;
	}
	ufbxt_assert(num_lines == error.stack_size + 1);
}
#endif

UFBXT_TEST(error_format_short)
#if UFBXT_IMPL
{
	char data[] = "Bad FBX";
	ufbx_error error;
	ufbx_scene *scene = ufbx_load_memory(data, sizeof(data), NULL, &error);
	ufbxt_assert(!scene);

	char error_buf[512];
	for (size_t buf_len = 0; buf_len <= ufbxt_arraycount(error_buf); buf_len++) {
		size_t ret_len = ufbx_format_error(error_buf, buf_len, &error);
		if (buf_len == 0) {
			ufbxt_assert(ret_len == 0);
			continue;
		}

		size_t str_len = strlen(error_buf);
		ufbxt_hintf("buf_len = %zu, ret_len = %zu, str_len = %zu", buf_len, ret_len, str_len);
		ufbxt_assert(ret_len == str_len);
		if (buf_len < 16) {
			ufbxt_assert(ret_len == buf_len - 1);
		}
	}
}
#endif

UFBXT_FILE_TEST(maya_node_attribute_zoo)
#if UFBXT_IMPL
{
	ufbx_node *node;

	ufbxt_assert(scene->settings.axes.right == UFBX_COORDINATE_AXIS_POSITIVE_X);
	ufbxt_assert(scene->settings.axes.up == UFBX_COORDINATE_AXIS_POSITIVE_Y);
	ufbxt_assert(scene->settings.axes.front == UFBX_COORDINATE_AXIS_POSITIVE_Z);
	ufbxt_assert(scene->settings.time_mode == UFBX_TIME_MODE_24_FPS);
	ufbxt_assert_close_real(err, scene->settings.unit_meters, 0.01f);
	ufbxt_assert_close_real(err, scene->settings.frames_per_second, 24.0f);

	node = ufbx_find_node(scene, "Null");
	ufbxt_assert(node && node->attrib_type == UFBX_ELEMENT_EMPTY);
	ufbxt_assert(node->attrib && node->attrib->type == UFBX_ELEMENT_EMPTY);

	node = ufbx_find_node(scene, "Mesh");
	ufbxt_assert(node && node->attrib_type == UFBX_ELEMENT_MESH);
	ufbxt_assert(node->attrib && node->attrib->type == UFBX_ELEMENT_MESH);
	ufbxt_assert(&node->mesh->element == node->attrib);

	node = ufbx_find_node(scene, "Bone");
	ufbxt_assert(node && node->attrib_type == UFBX_ELEMENT_BONE);
	ufbxt_assert(node->attrib && node->attrib->type == UFBX_ELEMENT_BONE);
	ufbxt_assert(&node->bone->element == node->attrib);
	ufbxt_assert_close_real(err, node->bone->radius, 0.5f);
	ufbxt_assert_close_real(err, node->bone->relative_length, 1.0f);

	node = ufbx_find_node(scene, "Camera");
	ufbxt_assert(node && node->attrib_type == UFBX_ELEMENT_CAMERA);
	ufbxt_assert(node->attrib && node->attrib->type == UFBX_ELEMENT_CAMERA);
	ufbxt_assert(&node->camera->element == node->attrib);
	ufbxt_assert(node->camera->gate_fit == UFBX_GATE_FIT_FILL);
	ufbxt_assert_close_real(err, node->camera->field_of_view_deg.x / 10.0f, 54.43f / 10.0f);
	ufbxt_assert_close_real(err, node->camera->focal_length_mm, 35.0f);

	node = ufbx_find_node(scene, "Light");
	ufbxt_assert(node && node->attrib_type == UFBX_ELEMENT_LIGHT);
	ufbxt_assert(node->attrib && node->attrib->type == UFBX_ELEMENT_LIGHT);
	ufbxt_assert(&node->light->element == node->attrib);
	ufbxt_assert_close_real(err, node->light->intensity, 1.0f);
	ufbxt_assert_close_real(err, node->light->color.x, 1.0f);
	ufbxt_assert_close_real(err, node->light->color.y, 1.0f);
	ufbxt_assert_close_real(err, node->light->color.z, 1.0f);

	node = ufbx_find_node(scene, "StereoCamera");
	ufbxt_assert(node && node->attrib_type == UFBX_ELEMENT_STEREO_CAMERA);
	ufbxt_assert(node->attrib && node->attrib->type == UFBX_ELEMENT_STEREO_CAMERA);
	ufbx_stereo_camera *stereo = (ufbx_stereo_camera*)node->attrib;
	ufbxt_assert(stereo->left && stereo->left->element.type == UFBX_ELEMENT_CAMERA);
	ufbxt_assert(stereo->right && stereo->right->element.type == UFBX_ELEMENT_CAMERA);
	ufbx_prop left_focal_prop = ufbx_evaluate_prop(&scene->anim, &stereo->left->element, "FocalLength", 0.5);
	ufbx_prop right_focal_prop = ufbx_evaluate_prop(&scene->anim, &stereo->right->element, "FocalLength", 0.5);
	ufbxt_assert_close_real(err, left_focal_prop.value_real, 42.011f);
	ufbxt_assert_close_real(err, right_focal_prop.value_real, 42.011f);

	node = ufbx_find_node(scene, "NurbsCurve");
	ufbxt_assert(node && node->attrib_type == UFBX_ELEMENT_NURBS_CURVE);
	ufbxt_assert(node->attrib && node->attrib->type == UFBX_ELEMENT_NURBS_CURVE);

	node = ufbx_find_node(scene, "NurbsSurface");
	ufbxt_assert(node && node->attrib_type == UFBX_ELEMENT_NURBS_SURFACE);
	ufbxt_assert(node->attrib && node->attrib->type == UFBX_ELEMENT_NURBS_SURFACE);

	node = ufbx_find_node(scene, "NurbsTrim");
	ufbxt_assert(node && node->attrib_type == UFBX_ELEMENT_NURBS_TRIM_SURFACE);
	ufbxt_assert(node->attrib && node->attrib->type == UFBX_ELEMENT_NURBS_TRIM_SURFACE);

	node = ufbx_find_node(scene, "LodGroup");
	ufbxt_assert(node && node->attrib_type == UFBX_ELEMENT_LOD_GROUP);
	ufbxt_assert(node->attrib && node->attrib->type == UFBX_ELEMENT_LOD_GROUP);
}
#endif

UFBXT_FILE_TEST(synthetic_duplicate_prop)
#if UFBXT_IMPL
{
	ufbx_prop *prop;
	ufbx_node *cube = ufbx_find_node(scene, "pCube1");
	ufbxt_assert(cube);

	prop = ufbx_find_prop(&cube->props, "Lcl Translation");
	ufbxt_assert(prop);
	ufbxt_assert_close_real(err, prop->value_vec3.x, -1.0f);

	prop = ufbx_find_prop(&cube->props, "Lcl Scaling");
	ufbxt_assert(prop);
	ufbxt_assert_close_real(err, prop->value_vec3.x, 2.0f);
}
#endif

UFBXT_FILE_TEST(synthetic_missing_version)
#if UFBXT_IMPL
{
	ufbxt_assert(scene->metadata.version == 6100);
	ufbxt_assert(scene->metadata.exporter == UFBX_EXPORTER_FBX_SDK);
	ufbxt_assert(scene->metadata.exporter_version == ufbx_pack_version(2007, 2, 28));
}
#endif

UFBXT_FILE_TEST(synthetic_missing_exporter)
#if UFBXT_IMPL
{
	ufbxt_assert(scene->metadata.version == 6100);
	ufbxt_assert(scene->metadata.exporter == UFBX_EXPORTER_UNKNOWN);
	ufbxt_assert(scene->metadata.exporter_version == 0);
}
#endif

UFBXT_FILE_TEST(synthetic_blender_old_exporter)
#if UFBXT_IMPL
{
	ufbxt_assert(scene->metadata.version == 6100);
	ufbxt_assert(scene->metadata.exporter == UFBX_EXPORTER_UNKNOWN);
	ufbxt_assert(scene->metadata.exporter_version == 0);
}
#endif

UFBXT_FILE_TEST(blender_272_cube)
#if UFBXT_IMPL
{
	ufbxt_assert(scene->metadata.version == 7400);
	ufbxt_assert(scene->metadata.exporter == UFBX_EXPORTER_BLENDER_BINARY);
	ufbxt_assert(scene->metadata.exporter_version == ufbx_pack_version(2, 72, 0));

	ufbxt_assert(!strcmp(scene->metadata.original_application.vendor.data, "Blender Foundation"));
	ufbxt_assert(!strcmp(scene->metadata.original_application.name.data, "Blender (stable FBX IO)"));
	ufbxt_assert(!strcmp(scene->metadata.original_application.version.data, "2.72 (sub 0)"));
	ufbxt_assert(!strcmp(scene->metadata.latest_application.vendor.data, "Blender Foundation"));
	ufbxt_assert(!strcmp(scene->metadata.latest_application.name.data, "Blender (stable FBX IO)"));
	ufbxt_assert(!strcmp(scene->metadata.latest_application.version.data, "2.72 (sub 0)"));
}
#endif

UFBXT_TEST(unicode_filename)
#if UFBXT_IMPL
{
	char buf[1024];
	int len = snprintf(buf, sizeof(buf), "%ssynthetic_\x61\xce\xb2\xe3\x82\xab\xf0\x9f\x98\x82_7500_ascii.fbx", data_root);
	ufbxt_assert(len > 0 && len < sizeof(buf));

	{
		ufbx_load_opts opts = { 0 };
		opts.ignore_geometry = true;

		ufbx_scene *scene = ufbx_load_file(buf, &opts, NULL);
		ufbxt_assert(scene);
		ufbxt_assert(ufbx_find_node(scene, "pCube1"));
		ufbxt_check_scene(scene);
		ufbx_free_scene(scene);
	}

	// Remove terminating \0 for explicit length test
	buf[len] = 'x';

	{
		ufbx_scene *scene = ufbx_load_file_len(buf, (size_t)len, NULL, NULL);
		ufbxt_assert(scene);
		ufbxt_assert(ufbx_find_node(scene, "pCube1"));
		ufbxt_check_scene(scene);
		ufbx_free_scene(scene);
	}
}
#endif

UFBXT_FILE_TEST(maya_cube_big_endian)
#if UFBXT_IMPL
{
	ufbxt_assert(scene->metadata.big_endian);
}
#endif

UFBXT_FILE_TEST(synthetic_cube_nan)
#if UFBXT_IMPL
{
	ufbx_node *node = ufbx_find_node(scene, "pCube1");
	ufbxt_assert(node && node->mesh);
	ufbx_mesh *mesh = node->mesh;
	ufbxt_assert(mesh->num_vertices >= 2);
	ufbxt_assert(isnan(mesh->vertices.data[0].x));
	ufbxt_assert(isinf(mesh->vertices.data[0].y) && mesh->vertices.data[0].y < 0.0f);
	ufbxt_assert(isinf(mesh->vertices.data[0].z) && mesh->vertices.data[0].z > 0.0f);
	ufbxt_assert_close_real(err, mesh->vertices.data[1].x, 0.5f);
	ufbxt_assert_close_real(err, mesh->vertices.data[1].y, -0.5f);
	ufbxt_assert_close_real(err, mesh->vertices.data[1].z, 0.5f);
}
#endif

UFBXT_FILE_TEST(synthetic_string_collision)
#if UFBXT_IMPL
{
	ufbx_node *node = ufbx_find_node(scene, "pCube1");
	ufbxt_assert(node);
	ufbx_prop *prop;

	prop = ufbx_find_prop(&node->props, "BIiMTNT");
	ufbxt_assert(prop);
	ufbxt_assert(!strcmp(prop->value_str.data, "BJKKUfZ"));

	prop = ufbx_find_prop(&node->props, "BbAJerP");
	ufbxt_assert(prop);
	ufbxt_assert(!strcmp(prop->value_str.data, "AAAJKop"));

	prop = ufbx_find_prop(&node->props, "BpgrcZR");
	ufbxt_assert(prop);
	ufbxt_assert(!strcmp(prop->value_str.data, "APGAmLj"));
}
#endif

UFBXT_FILE_TEST(synthetic_id_collision)
#if UFBXT_IMPL
{
	ufbxt_assert(scene->nodes.count == 10002);
	ufbx_node *node = ufbx_find_node(scene, "First");
	for (int32_t depth = 10000; depth >= 0; depth--) {
		ufbxt_assert(node);
		ufbxt_assert(node->node_depth == depth);
		node = node->parent;
	}
	ufbxt_assert(!node);
}
#endif

UFBXT_FILE_TEST_ALLOW_ERROR(synthetic_node_depth_fail)
#if UFBXT_IMPL
{
}
#endif

UFBXT_TEST(open_file)
#if UFBXT_IMPL
{
	const char *name = "maya_cube_7500_ascii.fbx";

	char buf[512];
	snprintf(buf, sizeof(buf), "%s%s", data_root, name);

	for (size_t i = 0; i < 2; i++) {
		ufbx_stream stream = { 0 };
		bool ok = ufbx_open_file(NULL, &stream, buf, i == 0 ? SIZE_MAX : strlen(buf));
		ufbxt_assert(ok);
		ufbxt_assert(stream.skip_fn);
		ufbxt_assert(stream.read_fn);
		ufbxt_assert(stream.close_fn);

		ufbxt_assert(stream.skip_fn(stream.user, 2));

		char result[3];
		size_t num_read = stream.read_fn(stream.user, result, 3);
		ufbxt_assert(num_read == 3);

		ufbxt_assert(!memcmp(result, "FBX", 3));

		stream.close_fn(stream.user);
	}

}
#endif

UFBXT_TEST(file_not_found)
#if UFBXT_IMPL
{
	const char *name = "maya_cube_9999_emoji.fbx";

	char buf[512];
	snprintf(buf, sizeof(buf), "%s%s", data_root, name);

	ufbx_error error;
	ufbx_scene *scene = ufbx_load_file(buf, NULL, &error);
	ufbxt_assert(!scene);
	ufbxt_assert(error.type == UFBX_ERROR_FILE_NOT_FOUND);
}
#endif

UFBXT_TEST(retain_scene)
#if UFBXT_IMPL
{
	char path[512];
	ufbxt_file_iterator iter = { "maya_cube" };
	while (ufbxt_next_file(&iter, path, sizeof(path))) {
		ufbx_load_opts opts = { 0 };

		ufbx_scene *scene = ufbx_load_file(path, &opts, NULL);
		ufbxt_assert(scene);

		ufbxt_check_scene(scene);

		ufbx_scene *eval_scene = ufbx_evaluate_scene(scene, NULL, 0.1, NULL, NULL);
		ufbx_free_scene(scene);

		// eval_scene should retain references
		ufbx_node *node = ufbx_find_node(eval_scene, "pCube1");
		ufbxt_assert(node);
		ufbxt_assert(!strcmp(node->name.data, "pCube1"));

		// No-op retain and release
		ufbx_retain_scene(eval_scene);
		ufbx_free_scene(eval_scene);

		ufbx_free_scene(eval_scene);
	}
}
#endif

UFBXT_FILE_TEST(maya_unicode)
#if UFBXT_IMPL
{
	// WHAT? Maya turns U+03B2 (Greek Small Letter Beta) into U+00DF (Latin Small Letter Sharp S)
	// The larger codepoints just get turned into one or two underscores which makes a bit more sense...
	ufbx_node *node = ufbx_find_node(scene, "\x61\xc3\x9f___");
	ufbxt_assert(node);
}
#endif

UFBXT_FILE_TEST(max_unicode)
#if UFBXT_IMPL
{
	ufbx_node *node = ufbx_find_node(scene, "\x61\xce\xb2\xe3\x82\xab\xf0\x9f\x98\x82");
	ufbxt_assert(node);
}
#endif

UFBXT_FILE_TEST(blender_279_unicode)
#if UFBXT_IMPL
{
	ufbx_node *node = ufbx_find_node(scene, "\x61\xce\xb2\xe3\x82\xab\xf0\x9f\x98\x82");
	ufbxt_assert(node);
}
#endif

#if UFBXT_IMPL
static uint32_t ufbxt_decode_hex_char(char c)
{
	if (c >= '0' && c <= '9') {
		return (uint32_t)c - '0';
	} else if (c >= 'a' && c <= 'f') {
		return (uint32_t)c - 'a' + 10;
	} else if (c >= 'A' && c <= 'F') {
		return (uint32_t)c - 'A' + 10;
	} else {
		ufbxt_assert(false && "Bad hex character");
		return 0;
	}
}

static size_t ufbxt_decode_hex(uint8_t *dst, size_t dst_len, ufbx_string src)
{
	ufbxt_assert(src.length % 2 == 0);

	size_t num = src.length / 2;
	ufbxt_assert(num <= dst_len);

	for (size_t i = 0; i < num; i++) {
		dst[i] = (uint8_t)(
			ufbxt_decode_hex_char(src.data[i * 2 + 0]) << 4u |
			ufbxt_decode_hex_char(src.data[i * 2 + 1]) << 0u );
	}

	return num;
}
#endif

UFBXT_FILE_TEST(synthetic_unicode)
#if UFBXT_IMPL
{
	uint8_t ref[128];

	{
		ufbx_node *node = ufbx_find_node(scene, "Good");
		ufbxt_assert(node);

		for (size_t prop_ix = 0; prop_ix < node->props.props.count; prop_ix++) {
			ufbx_prop *prop = &node->props.props.data[prop_ix];
			size_t len = ufbxt_decode_hex(ref, ufbxt_arraycount(ref), prop->name);

			ufbx_string src = prop->value_str;

			size_t src_ix = 0;
			for (size_t ref_ix = 0; ref_ix < len; ref_ix++) {
				uint8_t rc = ref[ref_ix];
				if (rc == 0) {
					ufbxt_assert(src_ix + 3 <= src.length);
					ufbxt_assert((uint8_t)src.data[src_ix + 0] == 0xef);
					ufbxt_assert((uint8_t)src.data[src_ix + 1] == 0xbf);
					ufbxt_assert((uint8_t)src.data[src_ix + 2] == 0xbd);
					src_ix += 3;
				} else {
					ufbxt_assert(src_ix < src.length);
					ufbxt_assert((uint8_t)src.data[src_ix] == rc);
					src_ix += 1;
				}
			}
			ufbxt_assert(src_ix == src.length);
		}
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "Bad");
		ufbxt_assert(node);

		for (size_t prop_ix = 0; prop_ix < node->props.props.count; prop_ix++) {
			ufbx_prop *prop = &node->props.props.data[prop_ix];
			size_t len = ufbxt_decode_hex(ref, ufbxt_arraycount(ref), prop->name);

			ufbx_string src = prop->value_str;
			const char *replacement = strstr(src.data, "\xef\xbf\xbd");
			ufbxt_assert(replacement);
		}
	}
}
#endif

UFBXT_FILE_TEST(max_quote)
#if UFBXT_IMPL
{
	{
		ufbx_node *node = ufbx_find_node(scene, "\"'&\"");
		ufbxt_assert(node);
		ufbxt_assert(node->mesh);
		ufbx_mesh *mesh = node->mesh;
		ufbxt_assert(mesh->materials.count == 1);
		ufbx_material *material = mesh->materials.data[0].material;
		if (scene->metadata.ascii) {
			ufbxt_assert(!strcmp(material->name.data, "&&q&qu&quo&quot\"\""));
		} else {
			ufbxt_assert(!strcmp(material->name.data, "&&q&qu&quo&quot&quot;\""));
		}

	}

	{
		ufbx_node *node = ufbx_find_node(scene, "\"");
		ufbxt_assert(node);
	}

	{
		ufbx_node *node = ufbx_find_node(scene, "&quot;");
		if (scene->metadata.ascii) {
			ufbxt_assert(!node);
		} else {
			ufbxt_assert(node);
		}
	}
}
#endif

UFBXT_FILE_TEST_ALLOW_ERROR(synthetic_truncated_quot_fail)
#if UFBXT_IMPL
{
}
#endif
