import httpx
from locust import HttpUser, task


def get_token_keycloak():
    url_token = 'https:**************************************************'
    headers_token = {'Content-Type': 'application/x-www-form-urlencoded'}
    data_token = {
        'grant_type': 'password',
        'username': '************',
        'password': '*****************',
        'client_id': 'admin-cli'
    }
    request = httpx.post(url_token, headers=headers_token, data=data_token)
    json = request.json()
    return json.get('access_token')


class LocustUser(HttpUser):
    token = ''
    headers = {}
    fixed_count = 1

    def __init__(self, *args, **kwargs):
        token = get_token_keycloak()
        self.token = token
        self.headers.update({'Authorization': 'Bearer ' + token})
        super().__init__(*args, **kwargs)

    @task
    def locust(self):
        response_project = self.client.post(
            f"/api/projects/", json={"name": "test project", "description": "test description", "type": "TEXT"}, headers=self.headers)
        project_id = response_project.json()["id"]

        self.client.get(f"/api/projects/{project_id}", headers=self.headers)

        response_directory = self.client.post(f"/api/audio_category/",
                                              json={
                                                  "name": "Справочник",
                                                  "color": "#a1a1a1",
                                                  "description": "",
                                                  "parent": None,
                                                  "project": f"https://******************/{project_id}/"},
                                              headers=self.headers)
        directory_id = response_directory.json()["id"]

        response_category = self.client.post(f"/api/audio_category/", json={
            "name": "Категория 1",
            "color": "#a1a1a1",
            "description": "",
            "parent": f"https:*****************/{directory_id}/",
            "project": f"https:*****************/{project_id}/"},
            headers=self.headers)


        self.client.delete(f"/api/projects/{project_id}", headers=self.headers)



