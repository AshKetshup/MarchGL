class cubeMarch {
public:
	float x_lim, y_lim, z_lim;
	float dist;
	int points;
	std::vector<float> cmgrid;
	Shader shader;
	GLuint VAO;
	
	cubeMarch(float x_lim, float y_lim, float z_lim, float dist) {
		this->x_lim = x_lim;
		this->y_lim = y_lim;
		this->z_lim = z_lim;
		this->dist = dist;
		createGrid();
	}
	
	void createGrid() {
		for (float x = -x_lim; x < x_lim; x += dist) {
			for (float y = -y_lim; y < y_lim; y += dist) {
				for (float z = -z_lim; z < z_lim; z+=dist) {
					cmgrid.push_back(x);
					cmgrid.push_back(y);
					cmgrid.push_back(z);
					
					cmgrid.push_back(1.0f);
					cmgrid.push_back(1.0f);
					cmgrid.push_back(1.0f);
				}
			}
		}
		printf("asdasd %d\n", cmgrid.size());
		points = cmgrid.size() / 6;

		GLuint VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);


		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		printf("%d\n", cmgrid.size());
		glBufferData(GL_ARRAY_BUFFER, cmgrid.size() * sizeof(float), &cmgrid[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

		//shadders
		shader = Shader("shaders/basicColorShader.vs", "shaders/basicColorShader.fs");
		if (!shader.wasSuccessful()) {
			cout << "Shader was not successful" << endl;
		}

	}
	void drawGrid(Camera camera) {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;

		projection = glm::perspective(glm::radians(camera.Zoom), (float)1920 / (float)1080, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
		model = glm::mat4(1.0f);
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		glBindVertexArray(VAO);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(5.0);
		glDrawArrays(GL_POINTS, 0, points);
	}


	void createMesh() {
		for (float x = -x_lim; x < x_lim; x += dist) {
			for (float y = -y_lim; y < y_lim; y += dist) {
				for (float z = -z_lim; z < z_lim; z += dist) {
					string cube = "00000000";
					
					//NOTE: see README for details
					vec3 p0 = vec3(x,y,z);
					vec3 p1 = vec3(x+dist, y, z);
					vec3 p2 = vec3(x+dist, y+dist, z);
					vec3 p3 = vec3(x, y+dist, z);
					vec3 p4 = vec3(x, y, z+dist);
					vec3 p5 = vec3(x + dist, y, z + dist);
					vec3 p6 = vec3(x + dist, y + dist, z + dist);
					vec3 p7 = vec3(x, y + dist, z + dist);

					//torus
					//x = (R1 + (R2 * cos(v))) * cos(u);
					//y = (R1 + (R2 * cos(v))) * sin(u);
					//z = R2 * sin(v);

				}
			}
		}
	}

};