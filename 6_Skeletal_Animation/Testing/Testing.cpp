void Animation::ProcessBones(const aiAnimation* animation, Model& model)
{
	int num_channels = animation->mNumChannels;
	unordered_map<string, BoneInfo>& bone_infos_map = model.GetBoneInfoMap();
	int num_bones = model.GetNumBones();

	for (int i = 0; i < num_channels; ++i)
	{
		auto channel = animation->mChannels[i];
		string bone_name = channel->mNodeName.data;

		if (bone_infos_map.find(bone_name) == bone_infos_map.end())
		{
			bone_infos_map[bone_name].id = num_bones;
			num_bones++;
		}
		m_bones.push_back(Bone(bone_name, bone_infos_map[bone_name].id, channel));
	}

	m_bone_info_map = bone_infos_map;
}

	cout << "Testing Matrix Multiplication" << endl;
	vec4 p = vec4(0.0f,0.0f, 0.0f, 1.0f);

	mat4 A = translate(mat4(1.0f), vec3(3.0f, 0.0f, 0.0));
	mat4 B = rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 C = rotate(A, radians(45.0f), vec3(0.0f, 1.0f, 0.0));
	mat4 D = translate(B, vec3(0.0f, 3.0f, 0.0f));

	cout << "A: " << endl;
	cout << A << endl;

	cout << "B: " << endl;
	cout << B << endl;
	
	cout << "A*B*p: " << endl;
	cout << A * B * p << endl;

	cout << "B*A: " << endl;
	cout << B * A * p << endl;