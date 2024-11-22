size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  std::ofstream *file = (std::ofstream *)userp;
  size_t totalSize = size * nmemb;
  file->write((char *)contents, totalSize);
  return totalSize;
}

void syncrepo(const std::string &url, const std::string &filename) {
  CURL *curl;
  CURLcode res;
  std::ofstream file(filename, std::ios::binary | std::ios::trunc);

  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << "Sync failed: " << curl_easy_strerror(res) << ". Package or repository not found."
                << std::endl;
      exit(404);
    } else {
      std::cout << "Successfully synced: " << url << " to " << filename
                << std::endl;
    }

    curl_easy_cleanup(curl);
  } else {
    std::cerr << "Failed to initialize CURL" << std::endl;
  }

  file.close();
  curl_global_cleanup();
}
