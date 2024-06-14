// import necessary modules
import { check } from "k6";
import http from "k6/http";

const root = "test/stress_test/";

// define configuration
export const options = {
  // define thresholds
  // thresholds: {
  //   // http_req_failed: ["rate<0.01"], // http errors should be less than 1%
  //   http_req_duration: ["p(99)<1000"], // 99% of requests should be below 1s
  // },
};

export default function () {
  // define URL and request body
  const url = `http://localhost:4242/cgi/`;
  const payload = "this is body data";
  const params = {
    headers: {
      "Content-Type": "text",
      "Content-Length": payload.length.toString(),
    },
  };

  const getRes = http.get(url);
  const postRes = http.post(url, payload, params);
  const delRes = http.del(url, payload, params);

  check(getRes, {
    "get: response code was 200": (getRes) => getRes.status == 200,
  });
  check(postRes, {
    "post: response code was 200": (postRes) => postRes.status == 200,
  });
  check(delRes, {
    "del: response code was 200": (delRes) => delRes.status == 200,
  });
}
