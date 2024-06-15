// import necessary modules
import { check } from "k6";
import http from "k6/http";

const root = "test/stress_test/";
const server_ip_addr = "http://localhost:4242";
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
  const staReq = `${server_ip_addr}/`;
  const dynReq = `${server_ip_addr}/cgi/`;
  const payload = "this is body data";
  const params = {
    headers: {
      "Content-Type": "text",
      "Content-Length": payload.length.toString(),
    },
  };

  const getResDyn = http.get(dynReq);
  const getResSta = http.get(staReq);
  const headRes = http.head(dynReq);
  const postRes = http.post(dynReq, payload, params);
  const delRes = http.del(dynReq, payload, params);

  // error response
  const errResNotFound = http.get(`${server_ip_addr}/not_found`);
  const errResNotAllowd = http.post(staReq, payload, params);

  check(getResDyn, {
    "get: response code was 200": (getRes) => getResDyn.status == 200,
  });
  check(getResSta, {
    "get: response code was 200": (getRes) => getResSta.status == 200,
  });
  check(headRes, {
    "head: response code was 200": (headRes) => headRes.status == 200,
  });
  check(postRes, {
    "post: response code was 200": (postRes) => postRes.status == 200,
  });
  check(delRes, {
    "del: response code was 200": (delRes) => delRes.status == 200,
  });

  check(errResNotFound, {
    "response code was 404": (errResNotFound) => errResNotFound.status == 404,
  });
  check(errResNotAllowd, {
    "response code was 405": (errResNotAllowd) => errResNotAllowd.status == 405,
  });
}
